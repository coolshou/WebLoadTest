@ECHO OFF
setlocal enabledelayedexpansion

set CREATEQIPERF=0
if "%1"=="daemon" (
    echo "only create daemon setup package"
) else (
    set CREATEQIPERF=1
)


echo "%PATH%" | findstr /c:"NSIS" >nul
if "%errorlevel%"=="1" (
    echo setting NSIS PATH
    set "PATH=C:\Program Files (x86)\NSIS\;%PATH%"
)

REM # get version from version.h
set "file=src\versions.h"
set "qiperf_pattern=#define WEBLOADTEST_VERSION"

for /f "tokens=3 delims=\ " %%i in ('findstr /c:"%qiperf_pattern%" "%file%"') do (
    set APPVERSION=%%i
    echo APPVERSION:!APPVERSION!
)

IF DEFINED APPVERSION (
    echo create webloadtest setup...
    echo "makensis.exe /DAPPVERSION=%APPVERSION% /DWIN64 /V4 WebLoadTest.nsi"
    makensis.exe /DAPPVERSION=%APPVERSION% /DWIN64 /V4 WebLoadTest.nsi
    
) ELSE (
    echo "Did not get APPVERSION"
)
