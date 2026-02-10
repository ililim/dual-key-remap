@echo off
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 -no_logo
cd /d "%~dp0"
cl tests.c && .\tests.exe
