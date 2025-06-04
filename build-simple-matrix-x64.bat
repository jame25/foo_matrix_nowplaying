@echo off
echo ========================================
echo Simple Matrix Now Playing Component Build (x64)
echo ========================================
echo.

REM Check if we're in a Visual Studio command prompt
where cl.exe >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: This script must be run from a Visual Studio Developer Command Prompt
    echo.
    echo Please open "x64 Native Tools Command Prompt for VS 2022" and run this script again
    pause
    exit /b 1
)

REM Check if we're in x64 mode
cl.exe 2>&1 | findstr /C:"x64" >nul
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: You may not be in x64 mode. Please ensure you're using:
    echo "x64 Native Tools Command Prompt for VS 2022"
    echo.
)

REM Create output directory
if not exist output_x64 mkdir output_x64

echo Compiling Matrix Now Playing component (x64) with Windows APIs...

REM Set paths
SET SDK_ROOT=%~dp0foobar2000_SDK
SET INCLUDES=/I"%SDK_ROOT%\foobar2000\SDK" /I"%SDK_ROOT%"

REM Compile all source files with x64 flags
echo Compiling main.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_WIN64 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output_x64\main.obj" ^
    "src\main.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: main.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling simple_matrix_client.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_WIN64 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output_x64\simple_matrix_client.obj" ^
    "src\simple_matrix_client.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: simple_matrix_client.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling play_callback.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_WIN64 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output_x64\play_callback.obj" ^
    "src\play_callback.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: play_callback.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling preferences.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_WIN64 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output_x64\preferences.obj" ^
    "src\preferences.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: preferences.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling darkmode_stub.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_WIN64 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output_x64\darkmode_stub.obj" ^
    "src\darkmode_stub.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: darkmode_stub.cpp compilation failed
    pause
    exit /b 1
)

REM Compile resource file if it exists
if exist "src\resource.rc" (
    echo Compiling resource file...
    rc.exe /fo"output_x64\resource.res" "src\resource.rc"
    if %ERRORLEVEL% NEQ 0 (
        echo WARNING: Resource compilation failed, continuing without resources
        set RESOURCE_FILE=
    ) else (
        set RESOURCE_FILE="output_x64\resource.res"
    )
) else (
    set RESOURCE_FILE=
)

REM Link with x64 SDK libraries and Windows APIs
echo Linking complete x64 component...
link.exe /nologo /DLL /LTCG /MACHINE:X64 ^
    /OUT:"output_x64\foo_matrix_nowplaying.dll" ^
    /LIBPATH:"%SDK_ROOT%\foobar2000\foobar2000_component_client\x64\Release" ^
    /LIBPATH:"%SDK_ROOT%\foobar2000\SDK\x64\Release" ^
    /LIBPATH:"%SDK_ROOT%\pfc\x64\Release" ^
    /LIBPATH:"%SDK_ROOT%\foobar2000\shared\x64\Release" ^
    "output_x64\main.obj" ^
    "output_x64\simple_matrix_client.obj" ^
    "output_x64\play_callback.obj" ^
    "output_x64\preferences.obj" ^
    "output_x64\darkmode_stub.obj" ^
    %RESOURCE_FILE% ^
    foobar2000_component_client.lib ^
    foobar2000_SDK.lib ^
    pfc.lib ^
    shared.lib ^
    user32.lib kernel32.lib winmm.lib ole32.lib shell32.lib shlwapi.lib ^
    wininet.lib advapi32.lib gdi32.lib

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Linking failed
    echo.
    echo Make sure you have built the x64 SDK projects first!
    echo Expected library paths:
    echo - %SDK_ROOT%\foobar2000\foobar2000_component_client\x64\Release\foobar2000_component_client.lib
    echo - %SDK_ROOT%\foobar2000\SDK\x64\Release\foobar2000_SDK.lib
    echo - %SDK_ROOT%\pfc\x64\Release\pfc.lib
    echo - %SDK_ROOT%\foobar2000\shared\x64\Release\shared.lib
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo x64 Build completed successfully!
echo ========================================
echo.
echo Output: output_x64\foo_matrix_nowplaying.dll
echo.
echo The x64 component now includes:
echo - Simple Matrix client using Windows WinINet APIs
echo - Matrix v3 API support (fixes HTTP 405 error)
echo - Play callback for track notifications  
echo - Preferences page for configuration
echo - No external dependencies (curl/jsoncpp)
echo.
echo To test: Copy the DLL to your foobar2000 x64 components folder
echo and configure Matrix settings in Preferences.
echo.
echo IMPORTANT: This x64 DLL is only compatible with 64-bit foobar2000!
echo For 32-bit foobar2000, use the regular build-simple-matrix.bat instead.
echo.
pause