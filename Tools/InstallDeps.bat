@echo off

REM Switch to the source directory
PUSHD "%~dp0..\Sources

REM Make sure the dependencies are extracted
echo Checking dependencies...
if not exist deps (
    echo Dependencies not found! 
    echo Unpacking Caveman2Cosmos DLL dependencies, please wait for the process to complete...
    start /b /wait deps.exe -y -o.
)
echo ...Done!

POPD
