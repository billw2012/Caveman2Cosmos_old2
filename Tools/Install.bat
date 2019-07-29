:: fchooser.bat
:: launches a folder chooser and outputs choice to the console
:: https://stackoverflow.com/a/15885133/1683264

@echo off

PUSHD "%~dp0"

setlocal

REM This is only used in the if block below, but needs to be here because otherwise it is a syntax error!
set "psCommand="(new-object -COM 'Shell.Application')^
.BrowseForFolder(0,'Please select your Beyond the Sword Mods folder',0,0).self.path""

REM install dir not set, ask user for it
if not exist mods_directory.txt (
    REM from https://stackoverflow.com/a/15885133
    for /f "usebackq delims=" %%I in (`powershell %psCommand%`) do set "folder=%%I"
    setlocal enabledelayedexpansion
    if "!folder!"=="" ( 
        echo No folder selected, cancelled!
        pause
        exit /b 1 
    )
    echo !folder! >mods_directory.txt
)

endlocal

set /p MODS_DIR=<mods_directory.txt
set MOD_DIR=%MODS_DIR%\Caveman2Cosmos
if exist "%MOD_DIR%" (
    echo Caveman2Cosmos already exists in mods folder at %MOD_DIR%!
    echo This operation will DELETE it and create it from scratch, are you SURE?
    echo To cancel close this window, or press Ctrl+C
    echo Or
    pause
)

REM rmdir /S/Q "%MOD_DIR%"
REM mkdir "%MOD_DIR%"

REM call PackFPKs.bat
REM call MakeDLLRelease.bat
REM call Copy.bat
