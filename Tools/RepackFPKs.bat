@echo off
cd /d "%~dp0"
set ASSETS_DIR=%~dp0..\Assets
set FPK_IN_DIR=%ASSETS_DIR%\unpacked

if exist "%FPK_IN_DIR%" (
    echo Directory %FPK_IN_DIR% already exists, were you part way through repacking? You better start again! Exiting...
    exit /B 1
)

mkdir "%FPK_IN_DIR%"
echo Unpacking existing FPKs into a temporary directory...
PakBuild /I="%ASSETS_DIR%" /O="%FPK_IN_DIR%" /F /U

echo Overlaying new art files...
xcopy /Y/S/R "%ASSETS_DIR%\art" "%FPK_IN_DIR%"

echo Repacking FPKs...
PakBuild /I="%FPK_IN_DIR%" /O="%ASSETS_DIR%" /F /S=50 /R=C2C /X=bik

echo Deleting temporary directory...
rmdir /Q/S "%FPK_IN_DIR%"

echo You can cleanup the art directory now, everything except those .bik files should be in the FPKs!
pause
