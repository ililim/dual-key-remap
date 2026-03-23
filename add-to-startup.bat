@echo off
setlocal enabledelayedexpansion

:: Get the directory where this script is located
set "SCRIPT_DIR=%~dp0"

:: Scoop-aware path resolution:
:: When installed via Scoop, this script lives in a versioned folder
:: (e.g. apps\dual-key-remap\0.11\). The sibling 'current' directory is
:: a symlink that always points at the active version. Using it means the
:: scheduled task survives 'scoop update' without needing to be re-registered.
set "SCOOP_CURRENT=%SCRIPT_DIR%..\current\"
if exist "%SCOOP_CURRENT%dual-key-remap.exe" (
    set "EXE_PATH=%SCOOP_CURRENT%dual-key-remap.exe"
) else (
    set "EXE_PATH=%SCRIPT_DIR%dual-key-remap.exe"
)

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
    echo ERROR: dual-key-remap.exe not found!
    echo Expected location: %EXE_PATH%
    echo.
    pause
    exit /b 1
)

echo This script creates a Windows scheduled task to automatically
echo start dual-key-remap when you log in with admin rights.
echo.
echo Registering task with exe at:
echo %EXE_PATH%
echo.

powershell.exe -NoProfile -Command ^
    "$exe = '%EXE_PATH%';" ^
    "$dir = Split-Path $exe;" ^
    "$action = New-ScheduledTaskAction -Execute $exe -WorkingDirectory $dir;" ^
    "$trigger = New-ScheduledTaskTrigger -AtLogOn -User $env:USERNAME;" ^
    "$principal = New-ScheduledTaskPrincipal -UserId $env:USERNAME -RunLevel Highest -LogonType Interactive;" ^
    "$settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit 0;" ^
    "Register-ScheduledTask -TaskName 'DualKeyRemap' -Action $action -Trigger $trigger -Principal $principal -Settings $settings -Force | Out-Null;" ^
    "Write-Host 'Done.'"

if !errorlevel! equ 0 (
    echo.
    echo Success! dual-key-remap will start automatically at login with admin privileges.
    echo To remove the startup task, run:
    echo     schtasks /delete /tn "DualKeyRemap" /f
    echo.
) else (
    echo.
    echo Task creation failed with error code: !errorlevel!
    echo.
)
