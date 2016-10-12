@echo off
rem This is just a one-stop file to run the vs2015 build script. Cleans then builds it again.
rem I suspect this will not work with any of the files in use. It should announce that when cleaning.

echo -- [ CLEANING ] --
cd Build
call clean.bat

echo.
echo.
echo -- [ BUILDING ] --
cd Build
echo Building...
call build_vs2015.bat

echo.
echo.
echo -- [ MOVING RESOURCES ] --
cd ..
copy Resources project_vs2015

echo.
echo.
echo -- [ EXITING ] --
timeout 30
