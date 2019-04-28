-- Premake5 Wiki: https://github.com/premake/premake-core/wiki
-- Based on Premake GLFW demo courtesy of JohannesMP
-- https://github.com/JohannesMP

function os.winSdkVersion()
  local reg_arch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
  local sdk_version = os.getWindowsRegistry( "HKLM:SOFTWARE" .. reg_arch .."Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )
  if sdk_version ~= nil then 
    return sdk_version 
  else
    return ""
  end
end

-- Variable definition: CHANGE THESE TO MODIFY PROJECT NAME
-- Brief reminder: This is actual lua, functions and whatnot are totally allowed.
local ROOT         = "../"          -- Path to project root


-- [ WORKSPACE CONFIGURATION ] --
workspace "ASCIIPlayer"                      -- Solution Name
    configurations { "Debug", "Release"}     -- Optimization/General config mode in VS
    platforms { --[["x64",]] "x86"}                -- Dropdown platforms section in VS
    location (ROOT .. "project_" .. _ACTION) -- Note: _ACTION is the argument passed to premake.

    -- [ PROJECT CONFIGURATION ] --
    project "ASCIIPlayer"        -- Project name
        targetname "ascii_player" -- Executable name
        kind "ConsoleApp"        -- Style of app in project- WindowedApp, ConsoleApp, etc.
        language "C++"

    -- [ COMPILER/LINKER CONFIG] --
    flags "FatalWarnings"  -- Warnings to 11! (all warnings on)

    -- Generate filters with info provided for Visual Studio
    filter { "platforms:*86" }
        architecture "x86"
    --filter { "platforms:*64" }
    --    architecture "x64"

    -- Generate configs dropdown info, VS
    filter { "configurations:Debug" }
        defines { "DEBUG" }  -- Actively defined in code, can be checked for.
        symbols "On"
    filter { "configurations:Release" }
        defines { "NDEBUG" } -- Actively defined in code, can be checked for.
        optimize "On"

    filter {} -- Reset filter.

    -- [ BUILD CONFIGURATIONS ] --

    filter {"system:macosx" } -- Mac uses clang.
        toolset "clang"
   
    filter { "action:gmake" }
        buildoptions { "-std=c++14" }

    -- Set the rpath on the executable, to allow for relative path for dynamic lib
    filter { "system:macosx", "toolset:clang or gcc" }
        linkoptions { "-rpath @executable_path/lib" }
    
    filter {"system:windows", "action:vs*"}
        linkoptions   { "/ignore:4099" }          -- Ignore library pdb warnings when running in debug
        systemversion(os.winSdkVersion() .. ".0") -- windows 10 SDK

    filter {} -- clear filter   


    -- [ FILE PATH CONFIGURATIONS ] --
    local output_dir_root         = ROOT .. "bin_%{cfg.platform}_%{cfg.buildcfg}_" .. _ACTION
    local output_dir_lib          = output_dir_root .. "/libs" -- Mac Specific
    targetdir(output_dir_root)    -- Where all output files are stored
    
    -- All files that we're currently going to worry about 
    local source_dir_root         = ROOT .. "Source"
    local source_dir_includes     = ROOT .. "External" .. "/**/Includes"
    local source_dir_libs         = ROOT .. "External" .. "/**/" .. "libs_" .. os.target()
    local source_dir_dependencies = ROOT .. "External"


	-- Includes and associated information directories 
    local source_dir_includes     = source_dir_dependencies .. "/**/Includes"
    local source_dir_libs         = source_dir_dependencies .. "/**/" .. "libs_" .. os.target()
    -- optional for libs that are 32 or 64 bit specific
    local source_dir_libs32       = source_dir_libs .. "/lib_x32"
    local source_dir_libs64       = source_dir_libs .. "/lib_x64"
    -- additional asset locations
    local resources_dir           = ROOT .. "Resources"

    -- Files to be compiled (cpp) or added to project (visual studio)
    files
    {
      source_dir_root .. "/**.c",
      source_dir_root .. "/**.h",
      source_dir_root .. "/**.cpp",
      source_dir_root .. "/**.hpp",
      source_dir_root .. "/**.tpp",
    }

    -- Omit templates from visual studio
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
    -- The LIB folder is an additional step to be added manually, creating an effective namespace.
    -- This prevents silly name overlapt, which we don't want to happen.
    includedirs
    {
      source_dir_includes,
      source_dir_root
    }
    
    configuration { "linux", "gmake" }
      linkoptions { "--verbose" }

    -- Where linker should look for library files
    -- NOTE: currently each library must have "LIBS_<OS>" in its path.
    libdirs
    {
      source_dir_dependencies,
      source_dir_libs,                                           -- default: look for any libs here (does not recurse)
      source_dir_libs .. "/lib_%{cfg.platform}",                 -- libs with ONLY x32/x64 (no Release/Debug) versions
      source_dir_libs .. "/%{cfg.buildcfg}",                     -- libs with ONLY Release/Debug (no x32/x64) versions
      source_dir_libs .. "/%{cfg.buildcfg}/lib_%{cfg.platform}", -- libs with BOTH Release/Debug AND x32/x64 versions
      source_dir_libs .. "/lib_%{cfg.platform}/%{cfg.buildcfg}", -- libs with BOTH x32/x64 AND Release/Debug versions (order reversed)
    }

	-- OS-specific Libraries - Dynamic libs will need to be copied to output
    -- WINDOWS
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

  
      -- MAC
    filter { "system:macosx", "configurations:Debug" }   
      links 
      { 
        "fmodexL" 
      }
    filter { "system:macosx", "configurations:Release" } 
      links 
      { 
        "fmodex" 
      }


      -- LINUX
    filter { "system:linux", "configurations:Debug" }
      links
      {
        "fmodexL"
      }
    filter { "system:linux", "configurations:Release" }
      links
      {
        "fmodex"
      }

    filter {}
