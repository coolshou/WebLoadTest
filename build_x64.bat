@ECHO OFF
setlocal EnableDelayedExpansion

set BUILDAPP=0
if "%1"=="setup" (
    echo only create setup package
) else (
    set BUILDAPP=1
)

if "%BUILDAPP%"=="1" (
    echo "Check Qt build environment"
    echo "%PATH%" | findstr /c:"Qt"
    if errorlevel 1 (
        set currentDir=%cd%
        set driveLetter=%currentDir:~0,2%
        echo set QT msvc2022_64 PATH
        REM this will enter another comspec!
        echo "C:\Qt\6.8.3\msvc2022_64\bin\qtenv2.bat"
        "C:\Qt\6.8.3\msvc2022_64\bin\qtenv2.bat"
        echo "back to %driveLetter%"
        %driveLetter%
    )
    echo "Check Visual Studio 2022 build environment"
    echo "%PATH%" | findstr /c:"Visual Studio"
    if errorlevel 1 (
        echo set Visual Studio 2022 Community PATH "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
        "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    )

    
    echo qmake...
    qmake
    echo nmake...
    nmake clean
    nmake
    windeployqt6.exe WebLoadTest_x86_64\WebLoadTest.exe

)
