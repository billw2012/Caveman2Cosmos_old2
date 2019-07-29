@echo off

REM Make sure the dependencies are extracted
PUSHD "%~dp0"
call InstallDeps.bat
POPD

REM Switch to the source directory
PUSHD "%~dp0..\Sources

REM Set the environment paths for the build
PUSHD "%~dp0..\Sources\deps\Microsoft Visual C++ Toolkit 2003"
SET PATH=%cd%\bin;%PATH%
SET INCLUDE=%cd%\include;%INCLUDE%
SET LIB=%cd%\lib;%LIB%
POPD

REM Call nmake to do the build, passing the first command line argument, which
REM   should be a build configuration name
echo Building DLL in %1 configuration ...
nmake Debug_clean
nmake Release_clean
nmake Final_Release_clean
echo ...Done!

REM Restore original directory
POPD
