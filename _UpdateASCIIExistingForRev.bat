@echo off
rem Terrible hardcoded batch file for moving the file I use to update my working version of ASCIIPlayer on just my machines.
rem You will inevitably forget that this favors the release build.

copy bin_x86_Debug_vs2017\ascii_player.exe C:\Users\rever\OneDrive\Programs\ASCIIPlayer\ascii_player.exe
copy bin_x86_Debug_vs2017\fmodexL.dll C:\Users\rever\OneDrive\Programs\ASCIIPlayer\fmodexL.dll
copy bin_x86_Release_vs2017\fmodex.dll C:\Users\rever\OneDrive\Programs\ASCIIPlayer\fmodex.dll
copy bin_x86_Release_vs2017\ascii_player.exe C:\Users\rever\OneDrive\Programs\ASCIIPlayer\ascii_player.exe
timeout 5