@echo off
setlocal enabledelayedexpansion

:: Get the directory where this script is located
set "SCRIPT_DIR=%~dp0"
set "EXE_PATH=%SCRIPT_DIR%dual-key-remap.exe"

echo ========================================
echo  Dual Key Remap - Add to startup
echo ========================================
echo.

:: Check if running as administrator
net session >nul 2>&1
if !errorlevel! neq 0 (
    echo ERROR: Script has been run without Administrator rights.
    echo Please right-click and select "Run as administrator".
    echo.
    echo Admin rights are needed so key remapping works in elevated windows
    echo ^(Task Manager, Registry Editor, etc^).
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

echo This script creates a Windows scheduled task to automatically
echo start dual-key-remap when you log in with admin rights.
echo.
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

powershell.exe -NoProfile -Command ^
    "$action = New-ScheduledTaskAction -Execute '\"%EXE_PATH%\"';" ^
    "$trigger = New-ScheduledTaskTrigger -AtLogOn;" ^
    "$principal = New-ScheduledTaskPrincipal -UserId '%USERNAME%' -RunLevel Highest -LogonType Interactive;" ^
    "$settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit 0;" ^
    "Unregister-ScheduledTask -TaskName 'DualKeyRemap' -Confirm:$false -ErrorAction SilentlyContinue;" ^
    "Register-ScheduledTask -TaskName 'DualKeyRemap' -Action $action -Trigger $trigger -Principal $principal -Settings $settings -Force"

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
