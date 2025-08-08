@echo off
setlocal enabledelayedexpansion

:: Get the directory where this script is located
set "SCRIPT_DIR=%~dp0"
set "EXE_PATH=%SCRIPT_DIR%dual-key-remap.exe"

echo ========================================
echo  Dual Key Remap - Add to startup
echo ========================================
echo.
echo This convenience script will create a Windows scheduled task to automatically
echo start dual-key-remap when you log in with admin rights.
echo.
echo Why administrator privileges are required:
echo   - To remap keys when the active window is an admin app
echo   - Examples: Task Manager, Registry Editor, Command Prompt (admin)
echo   - Without admin rights, key remapping won't work in these apps
echo.

:: Check if running as administrator
net session >nul 2>&1
if !errorlevel! neq 0 (
    echo This script needs to run as Administrator to create the scheduled task.
    echo Please right-click and select "Run as administrator"
    echo.
    pause
    exit /b 1
)

:: Check if dual-key-remap.exe exists
if not exist "%EXE_PATH%" (
    echo ERROR: dual-key-remap.exe not found in script directory!
    echo Expected location: %EXE_PATH%
    echo.
    pause
    exit /b 1
)

echo Found dual-key-remap.exe at:
echo %EXE_PATH%
echo.
set /p "CONFIRM=Is this the correct location? (Y/n): "
if /i "%CONFIRM%"=="n" (
    echo.
    echo Installation cancelled.
    pause
    exit /b 0
)
if /i "%CONFIRM%"=="no" (
    echo.
    echo Installation cancelled.
    pause
    exit /b 0
)
echo.

echo Creating scheduled task...
echo.

:: Delete existing task if it exists (ignore errors)
schtasks /delete /tn "DualKeyRemap" /f >nul 2>&1

:: Create the scheduled task
schtasks /create /tn "DualKeyRemap" /tr "\"%EXE_PATH%\"" /sc onlogon /rl highest /f /ru "%USERNAME%" /it

if !errorlevel! equ 0 (
    echo.
    echo The task will start automatically when you log in with admin privileges.
    echo To remove this task later, run:
    echo     schtasks /delete /tn "DualKeyRemap" /f
    echo.
) else (
    echo.
    echo Task creation failed with error code: !errorlevel!
    echo.
)

pause
