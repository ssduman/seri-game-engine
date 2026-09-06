@echo off
setlocal

echo === generating projects...
call script\premake5\premake5.exe vs2026
if errorlevel 1 (
  echo [error] premake failed.
  exit /b 1
)
echo === projects generated.

echo === done.
pause
