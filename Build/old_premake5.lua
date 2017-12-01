-- Premake5 Wiki: https://github.com/premake/premake-core/wiki
-- Based on Floofs_Inc Premake GLFW demo courtesy of JohannesMP


-- Variable definition
-- Brief reminder: This is actual lua, functions and whatnot are totally allowed.
local ROOT = "../"   -- Path to project root

---------------------------------
-- [ WORKSPACE CONFIGURATION   --
---------------------------------
workspace "ASCII_Player"                     -- Solution Name
    configurations { "Debug", "Release"}     -- Optimization/General config mode in VS
    platforms { "x64", "x86"}                -- Dropdown platforms section in VS
    location (ROOT .. "project_" .. _ACTION) -- Note: _ACTION is the argument passed to premake.

    -------------------------------
    -- [ PROJECT CONFIGURATION ] --
    ------------------------------- 
    project "ASCIIPlayer"          -- Project name
        targetname "ascii_player"  -- Executable name
        kind "ConsoleApp"          -- Style of app in project- WindowedApp, ConsoleApp, etc.
        language "C++"

    -------------------------------
    -- [ COMPILER/LINKER CONFIG] --
    -------------------------------
    
    -- Set compiler flags
    flags "FatalWarnings"  -- Warnings to 11! (all warnings on)

    -- Generate filters with info provided, VS
    filter { "platforms:*86" }
      architecture "x86"
    filter { "platforms:*64" }
      architecture "x64"

    -- Generate configs dropdown info, VS
    filter { "configurations:Debug" }
      defines { "DEBUG" }  -- Actively defined in code, can be checked for.
      --flags { "" }
    filter { "configurations:Release" }
      defines { "NDEBUG" } -- Actively defined in code, can be checked for.
      optimize "On"

    -- Reset filter.
    filter {}

    ------------------------------
    -- [ BUILD CONFIGURATIONS ] --
    ------------------------------
    local cur_toolset = "default" -- workaround for premake issue #257

    filter {"system:macosx" }           -- Mac uses clang.
      toolset "clang"
      cur_toolset = "clang"
   
    filter { "action:gmake" }
      buildoptions { "-std=c++14" }

    -- Set the rpath on the executable, to allow for relative path for dynamic lib
    filter { "system:macosx" }
      if cur_toolset == "clang" or cur_toolset == "gcc" then  
        linkoptions { "-rpath @executable_path/lib" }
      end
    
    filter {"system:windows", "action:vs*"}
      linkoptions   { "/ignore:4099" }      -- Ignore library pdb warnings when running in debug

    filter {} -- clear filter   






    ----------------------------------
    -- [ FILE PATH CONFIGURATIONS ] --
    ----------------------------------
    local output_dir_root         = ROOT .. "bin_%{cfg.platform}_%{cfg.buildcfg}_" .. _ACTION
    targetdir(output_dir_root )    -- Where all output files are stored
    local output_dir_lib          = output_dir_root .. "/libs" -- Mac Specific

    local source_dir_root         = ROOT .. "Source"
    local source_dir_dependencies = ROOT .. "External"


    -- Includes and associated information directories 
    local source_dir_includes     = source_dir_dependencies .. "/**/Includes"
    local source_dir_libs         = source_dir_dependencies .. "/**/" .. "Libs_" .. os.get()
    -- optional for libs that are 32 or 64 bit specific
    local source_dir_libs32       = source_dir_libs .. "/lib_x32"
    local source_dir_libs64       = source_dir_libs .. "/lib_x64"
    -- additional asset locations
    local resources_dir           = ROOT .. "Resources"

    -- Files to be compiled (cpp) or added to project (visual studio)
    files
    {
      source_dir_root .. "/**.cpp",
      source_dir_root .. "/**.hpp",
      source_dir_root .. "/**.tpp",
    }

    -- NO TEMPLATES FOR YOU!
    filter { "files:**.tpp" }
      flags {"ExcludeFromBuild"}
    filter {}

    -- Ignore files for other operating systems (not necessary in this project)
    filter { "system:macosx" } removefiles { source_dir_root .. "/**_windows.*" }
    filter { "system:windows" } removefiles { source_dir_root .. "/**_macosx.*"  }
    filter {} -- reset filter

    -- Organize VS filters
    vpaths {
      ["Headers"] = 
          { 
            source_dir_root .. "**.h", 
            source_dir_root .. "**.hpp"
          },
      ["Sources/*"] = 
        {
          source_dir_root .. "**.c", 
          source_dir_root .. "**.cpp", 
          source_dir_root .. "**.tpp"
        },
      ["Docs"] = "**.txt"
    }

    -- Where compiler should look for library includes
    -- NOTE: for library headers always use  '#include <LIB/lib.hpp>' not quotes
    -- Tge LIB folder is an additional step to be added manually, creating an effective namespace.
    -- This prevents silly name overlapt, which we don't want to happen.
    includedirs
    {
      source_dir_includes,
      source_dir_root
    }
    

    -- Where linker should look for library files
    -- NOTE: currently each library must have "LIBS_<OS>" in its path.
    libdirs
    {
      source_dir_libs,                                           -- default: look for any libs here (does not recurse)
      source_dir_libs .. "/lib_%{cfg.platform}",                 -- libs with ONLY x32/x64 (no Release/Debug) versions
      source_dir_libs .. "/%{cfg.buildcfg}",                     -- libs with ONLY Release/Debug (no x32/x64) versions
      source_dir_libs .. "/%{cfg.buildcfg}/lib_%{cfg.platform}", -- libs with BOTH Release/Debug AND x32/x64 versions
      source_dir_libs .. "/lib_%{cfg.platform}/%{cfg.buildcfg}"  -- libs with BOTH x32/x64 AND Release/Debug versions (order reversed)
    }
    
  
    -- OS-specific Libraries - Dynamic libs will need to be copied to output
    -- WINDOWS INCLUDES, 32 THEN 64 BIT, EACH BEING DEBUG AND RELEASE
    filter { "system:windows", "platforms:*86" , "configurations:Debug" }   
      links 
      { 
        "fmodexL_vc" 
      }
    filter { "system:windows", "platforms:*86" , "configurations:Release" } 
      links 
      { 
        "fmodex_vc" 
      }
    filter { "system:windows", "platforms:*64" , "configurations:Debug" }   
      links 
      { 
        "fmodexL64_vc" 
      }
    filter { "system:windows", "platforms:*64" , "configurations:Release" } 
      links 
      { 
        "fmodex64_vc" 
      }

  
      -- MAC INCLUDES, BEING DEBUG AND RELEASE. RECALL MAC IS ONLY 64-BIT
    filter { "system:macosx", "configurations:Debug" }   
      links 
      { 
        "libfmodexL" 
      }
    filter { "system:macosx", "configurations:Release" } 
      links 
      { 
        "libfmodex" 
      }

    filter {}


-- Automagical path translating

--[[
-----------------------------------
-- POST-BUILD CONFIGURATIONS, RIPPED DIRECTLY FROM GLFW DEMO
-----------------------------------
    -- Setting up cross-platform file manipulation commands
    -- NOTE: this is what premake5's tokens -should- before, but see issue #280
    local CWD       = "cd " .. os.getcwd() .. "; " -- We are in current working directory
    local MKDIR     = "mkdir -p "
    local COPY      = "cp -rf "

    local SEPARATOR = "/"

    if(os.get() == "windows") then
      CWD       = "chdir " .. os.getcwd() .. "\\.." .. " && "
      MKDIR     = "mkdir "
      COPY      = "xcopy /Q /E /Y /I "
      SEPARATOR = "\\"
    end

    -- mac copies dylibs to output dir
    -- Additonally, copies resources.
    filter { "system:macosx" }
      postbuildcommands
      {
        --path.translate ( CWD .. MKDIR .. output_dir_lib, SEPARATOR ),
      --  path.translate ( CWD .. COPY .. source_dir_dependencies .. "/*/Libs_macosx/*.dylib " .. output_dir_lib, SEPARATOR )
      }

    -- windows copies dll's to output dir (currently not used)
    -- Additoonally, copies resources.
    filter { "system:windows" }
      postbuildcommands
      {
      -- Janky garbage for moving resources folder over into regular stuff folder. This is pretty crap as far as automated building goes.
       -- path.translate ( CWD .. COPY .. source_dir_dependencies .. "/*/Libs_windows/*.dll " .. output_dir_root , SEPARATOR )
      }


    -- Copying resource files to output dir (currently not used)
    filter {}
    postbuildcommands
    { 
      --path.translate("chdir " .. os.getcwd() .. "\\.."),
      --path.translate("xcopy" .. resources_dir .. " " .. output_dir_root)

       
      --path.translate ( CWD .. COPY .. <RESOURCE_PATH> .. "/* " .. output_dir_root , SEPARATOR )
      --path.translate ( CWD .. COPY .. resources_dir .. "/*" .. output_dir_root, SEPARATOR )
    }


]]