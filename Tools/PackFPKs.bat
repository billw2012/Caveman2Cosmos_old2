@echo off
cd /d "%~dp0"
set ASSETS_DIR=%~dp0..\Assets
set FPK_IN_DIR=%ASSETS_DIR%\unpacked

if not exist %FPK_IN_DIR% (
    echo Directory %FPK_IN_DIR% doesn't exist, did you unpack first?
    exit /b 1
)

echo Packing FPKs...
PakBuild /I="%FPK_IN_DIR%" /O="%ASSETS_DIR%" /F /S=50 /R=C2C /X=bik

echo Deleting temporary directory...
rmdir /Q/S "%FPK_IN_DIR%"

echo You can cleanup the art directory now, everything except those .bik files should be in the FPKs!
pause
