@echo off
rem This is just a one-stop file to run the vs2017 build script. Cleans then builds it again, but doesn't work if the files are in use.

echo -- [ CLEANING ] --
cd Build
call clean.bat

echo.
echo.
echo -- [ BUILDING ] --
cd Build
echo Building...
call build_vs2017.bat

echo.
echo.
echo -- [ MOVING RESOURCES ] --
cd ..
copy Resources project_vs2017

echo.
echo.
echo -- [ EXITING ] --
timeout 30
