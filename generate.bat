@echo off
setlocal

echo === getting vcpkg...
if defined VCPKG_ROOT (
  set "VCPKG_EXE=%VCPKG_ROOT%\vcpkg.exe"
  echo [info] vcpkg set to "%VCPKG_EXE%".
) else (
  echo [error] vcpkg not found.
  exit /b 1
)

echo === restoring vcpkg dependencies...
call "%VCPKG_EXE%" install --triplet=x64-windows --no-print-usage
if errorlevel 1 (
  echo [error] vcpkg install failed.
  exit /b 1
) else (
  echo [info] vcpkg install succeeded.
)

echo === generating projects...
call script\premake5\premake5.exe vs2026
if errorlevel 1 (
  echo [error] premake failed.
  exit /b 1
) else (
  echo [info] projects generated.
)

echo === done.
pause
