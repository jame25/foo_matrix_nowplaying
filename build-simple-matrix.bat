@echo off
echo ========================================
echo Simple Matrix Now Playing Component Build
echo ========================================
echo.

REM Check if we're in a Visual Studio command prompt
where cl.exe >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: This script must be run from a Visual Studio Developer Command Prompt
    echo.
    echo Please open "Developer Command Prompt for VS 2022" and run this script again
    pause
    exit /b 1
)

REM Create output directory
if not exist output mkdir output

echo Compiling Matrix Now Playing component with Windows APIs...

REM Set paths
SET SDK_ROOT=E:\foo_matrix_nowplaying\foobar2000_SDK
SET INCLUDES=/I"%SDK_ROOT%\foobar2000\SDK" /I"%SDK_ROOT%"

REM Compile all source files
echo Compiling main.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output\main.obj" ^
    "src\main.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: main.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling simple_matrix_client.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output\simple_matrix_client.obj" ^
    "src\simple_matrix_client.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: simple_matrix_client.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling play_callback.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output\play_callback.obj" ^
    "src\play_callback.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: play_callback.cpp compilation failed
    pause
    exit /b 1
)

echo Compiling preferences.cpp...
cl.exe /nologo /c /MD /O2 /W3 /EHsc /std:c++17 ^
    /D_WINDOWS /DWIN32 /D_USRDLL /DNDEBUG /DUNICODE /D_UNICODE ^
    %INCLUDES% ^
    /Fo"output\preferences.obj" ^
    "src\preferences.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: preferences.cpp compilation failed
    pause
    exit /b 1
)

REM Compile resource file if it exists
if exist "src\resource.rc" (
    echo Compiling resource file...
    rc.exe /fo"output\resource.res" "src\resource.rc"
    if %ERRORLEVEL% NEQ 0 (
        echo WARNING: Resource compilation failed, continuing without resources
        set RESOURCE_FILE=
    ) else (
        set RESOURCE_FILE="output\resource.res"
    )
) else (
    set RESOURCE_FILE=
)

REM Link with SDK libraries and Windows APIs only
echo Linking complete component...
link.exe /nologo /DLL /LTCG ^
    /OUT:"output\foo_matrix_nowplaying.dll" ^
    /LIBPATH:"%SDK_ROOT%\foobar2000\foobar2000_component_client\Release" ^
    /LIBPATH:"%SDK_ROOT%\foobar2000\SDK\Release" ^
    /LIBPATH:"%SDK_ROOT%\pfc\Release" ^
    /LIBPATH:"%SDK_ROOT%\foobar2000\shared\Release" ^
    "output\main.obj" ^
    "output\simple_matrix_client.obj" ^
    "output\play_callback.obj" ^
    "output\preferences.obj" ^
    %RESOURCE_FILE% ^
    foobar2000_component_client.lib ^
    foobar2000_SDK.lib ^
    pfc.lib ^
    shared.lib ^
    user32.lib kernel32.lib winmm.lib ole32.lib shell32.lib shlwapi.lib ^
    wininet.lib

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Linking failed
    echo Make sure you have built the SDK projects first!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Output: output\foo_matrix_nowplaying.dll
echo.
echo The component now includes:
echo - Simple Matrix client using Windows WinINet APIs
echo - Matrix v3 API support (fixes HTTP 405 error)
echo - Play callback for track notifications  
echo - Preferences page for configuration
echo - No external dependencies (curl/jsoncpp)
echo.
echo To test: Copy the DLL to your foobar2000 components folder
echo and configure Matrix settings in Preferences.
echo.
pause