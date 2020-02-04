@echo off
rem This is just a one-stop file to run the vs2019 build script. Cleans then builds it again, but doesn't work if the files are in use.
rem If there is another build format that's desired, check out the build folder.

echo -- [ CLEANING ] --
cd Build
call clean.bat

echo.
echo.
echo -- [ BUILDING ] --
cd Build
echo Building...
call build_vs2019.bat

echo.
echo.
echo -- [ MOVING RESOURCES ] --
cd ..
copy Resources project_vs2019
copy Resources\icon project_vs2019
copy Resources\dll project_vs2019
copy Resources\samples project_vs2019

echo.
echo.
echo -- [ EXITING ] --
timeout 30
