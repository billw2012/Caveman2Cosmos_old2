@echo off
cd /d "%~dp0"
set ASSETS_DIR=%~dp0..\Assets
set ASSETS_OUT_DIR=%ASSETS_DIR%\unpacked

if exist "%ASSETS_OUT_DIR%" (
    echo Directory %ASSETS_OUT_DIR% already exists, did you already unpack?
    exit /B 1
    REM rmdir /S/Q "%ASSETS_OUT_DIR%"
    REM mkdir "%ASSETS_OUT_DIR%"
)

PakBuild /I="%ASSETS_DIR%" /O="%ASSETS_DIR%\unpacked" /U
REM for /f %%f in ("%ASSETS_DIR%\*.FPK") do (
REM     echo Unpacking %%f ...
REM     PakBuild /I="%%f" /O="%ASSETS_DIR%\unpacked"
REM )
