@echo off
setlocal enabledelayedexpansion
title SlayHer Auto Builder
echo =======================================
echo SlayHer Auto Builder - Building SlayHer.exe
echo =======================================
echo.

REM --- CONFIG ---
set SRC_DIR=%CD%\main
set OUT_EXE=%CD%\SlayHer.exe
set ICON_SRC=%CD%\resources\assets\icon.ico
set ICON_DEST=%SRC_DIR%\icon.ico
set ICON_RC=%SRC_DIR%\icon.rc
set ICON_OBJ=%SRC_DIR%\icon.o

set MINGW_ZIP=mingw64.zip
set MINGW_TEMP=mingw64_temp
set MINGW_DIR=%CD%\mingw64

REM --- CHECK FOR GCC ---
where gcc >nul 2>nul
if %errorlevel%==0 (
    echo GCC already installed.
    set MINGW_READY=1
) else (
    echo GCC not found. Will install MinGW64.
    set MINGW_READY=0
)

REM --- DOWNLOAD + EXTRACT MinGW64 IF MISSING ---
if "%MINGW_READY%"=="0" (
    if not exist "%MINGW_DIR%\bin\gcc.exe" (
        REM Download MinGW64 only if zip not present
        if not exist "%MINGW_ZIP%" (
            echo Downloading MinGW64...
            curl -L "https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-12.0.0-ucrt-r3/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64ucrt-12.0.0-r3.zip" -o "%MINGW_ZIP%"
            if errorlevel 1 (
                echo Failed to download MinGW64!
                pause
                exit /b 1
            )
        ) else (
            echo Found existing %MINGW_ZIP%, will use it.
        )

        REM Extract to temp folder first
        if exist "%MINGW_TEMP%" rmdir /s /q "%MINGW_TEMP%"
        mkdir "%MINGW_TEMP%"
        echo Extracting MinGW64...
        powershell -NoProfile -Command "Expand-Archive -LiteralPath '%MINGW_ZIP%' -DestinationPath '%MINGW_TEMP%' -Force"
        if errorlevel 1 (
            echo Failed to extract MinGW64!
            pause
            exit /b 1
        )

        REM Move extracted folder into MINGW_DIR
        for /d %%D in ("%MINGW_TEMP%\*") do (
            move "%%D" "%MINGW_DIR%"
        )
        rmdir /s /q "%MINGW_TEMP%"
        echo MinGW64 ready at %MINGW_DIR%
    ) else (
        echo MinGW64 already extracted at %MINGW_DIR%
    )

    REM Add MinGW64 to PATH temporarily
    set "PATH=%MINGW_DIR%\bin;%PATH%"
)

REM --- VERIFY GCC WORKS ---
gcc --version >nul 2>nul
if errorlevel 1 (
    echo GCC not found or failed to run!
    pause
    exit /b 1
)
echo GCC is ready.

REM --- COPY ICON ---
set ICON_COPIED=0
if exist "%ICON_SRC%" (
    echo Copying icon...
    copy /Y "%ICON_SRC%" "%ICON_DEST%" >nul
    if errorlevel 1 (
        echo Failed to copy icon!
        pause
        exit /b 1
    )
    set ICON_COPIED=1
) else (
    echo No icon found, will build without icon.
)

REM --- COMPILE GAME ---
echo Building SlayHer.exe...
if "%ICON_COPIED%"=="1" (
    echo Creating icon resource...
    echo 1 ICON "%ICON_DEST%" > "%ICON_RC%"
    windres "%ICON_RC%" "%ICON_OBJ%"
    if exist "%ICON_OBJ%" (
        gcc "%SRC_DIR%\*.c" "%ICON_OBJ%" -o "%OUT_EXE%"
    ) else (
        echo Icon object not created. Building without icon.
        gcc "%SRC_DIR%\*.c" -o "%OUT_EXE%"
    )
) else (
    gcc "%SRC_DIR%\*.c" -o "%OUT_EXE%"
)

if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

REM --- CLEANUP ---
if exist "%ICON_OBJ%" del "%ICON_OBJ%"
if exist "%ICON_RC%" del "%ICON_RC%"
if exist "%ICON_DEST%" del "%ICON_DEST%"

REM Remove MinGW64 zip and extracted folder
if exist "%MINGW_ZIP%" del /f "%MINGW_ZIP%"
if exist "%MINGW_DIR%" rmdir /s /q "%MINGW_DIR%"

echo.
echo =======================================
echo SlayHer.exe built successfully!
echo Output: %OUT_EXE%
echo =======================================
pause
endlocal
exit /b