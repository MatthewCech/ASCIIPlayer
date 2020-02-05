@echo off
:: Previously called "AddIconMoveResourcesvs2019.bat"
echo -- [ APPLYING TO 32-BIT DEBUG ] --
IF EXIST bin_x86_Debug_vs2019 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x86_Debug_vs2019/asciiplayer.exe", "bin_x86_Debug_vs2019/asciiplayer.exe", "Resources/icon/layered-256-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources\dll bin_x86_Debug_vs2019
  copy Resources\samples bin_x86_Debug_vs2019
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 32-BIT RELEASE] --
IF EXIST bin_x86_Release_vs2019 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x86_Release_vs2019/asciiplayer.exe", "bin_x86_Release_vs2019/asciiplayer.exe", "Resources/icon/layered-256-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources\dll bin_x86_Release_vs2019
  copy Resources\samples bin_x86_Release_vs2019
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 64-BIT DEBUG] --
IF EXIST bin_x64_Debug_vs2019 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x64_Debug_vs2019/asciiplayer.exe", "bin_x64_Debug_vs2019/asciiplayer.exe", "Resources/icon/layered-256-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources\dll bin_x64_Debug_vs2019
  copy Resources\samples bin_x64_Debug_vs2019
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 64-BIT RELEASE] --
IF EXIST bin_x64_Release_vs2019 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x64_Release_vs2019/asciiplayer.exe", "bin_x64_Release_vs2019/asciiplayer.exe", "Resources/icon/layered-256-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources\dll bin_x64_Release_vs2019
  copy Resources\samples bin_x64_Release_vs2019
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ UPDATING EXPLORER ICON CACHE ] --
call ie4uinit.exe -ClearIconCache

echo.
echo.
echo -- [ EXITING ] --
timeout 5