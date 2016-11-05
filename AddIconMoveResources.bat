@echo off
echo -- [ APPLYING TO 32-BIT DEBUG ] --
IF EXIST bin_x86_Debug_vs2015 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x86_Debug_vs2015/ascii_player.exe", 	        "bin_x86_Debug_vs2015/ascii_player.exe", "Resources/ASCII_Player.ico", ICONGROUP, MAINICON, 0
  copy Resources bin_x86_Debug_vs2015 
) ELSE ( 
  echo Directory does not exist. 
)

echo.
echo.
echo -- [ APPLYING TO 32-BIT RELEASE] --
IF EXIST bin_x86_Release_vs2015 (
  call "Build/RH/ResourceHacker.exe" -addoverwrite "bin_x86_Release_vs2015/ascii_player.exe",     "bin_x86_Release_vs2015/ascii_player.exe", "Resources/ASCII_Player.ico", ICONGROUP, MAINICON, 0
  copy Resources bin_x86_Release_vs2015
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