@echo off
:: Previously called "AddIconMoveResourcesvs2017.bat"
echo -- [ APPLYING TO 32-BIT DEBUG ] --
IF EXIST bin_x86_Debug_vs2017 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x86_Debug_vs2017/ascii_player.exe", "bin_x86_Debug_vs2017/ascii_player.exe", "Resources/icon-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources bin_x86_Debug_vs2017
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 32-BIT RELEASE] --
IF EXIST bin_x86_Release_vs2017 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x86_Release_vs2017/ascii_player.exe", "bin_x86_Release_vs2017/ascii_player.exe", "Resources/icon-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources bin_x86_Release_vs2017
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 64-BIT DEBUG] --
IF EXIST bin_x64_Debug_vs2017 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x64_Debug_vs2017/ascii_player.exe", "bin_x64_Debug_vs2017/ascii_player.exe", "Resources/icon-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources bin_x64_Debug_vs2017
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 64-BIT RELEASE] --
IF EXIST bin_x64_Release_vs2017 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x64_Release_vs2017/ascii_player.exe", "bin_x64_Release_vs2017/ascii_player.exe", "Resources/icon-128-64-48-32-16.ico", ICONGROUP, MAINICON, 0
  copy Resources bin_x64_Release_vs2017
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