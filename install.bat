@echo off
setlocal

echo === getting vcpkg...
if defined VCPKG_ROOT (
  set "VCPKG_EXE=%VCPKG_ROOT%\vcpkg.exe"
) else (
  echo [error] "VCPKG_ROOT" not found.
  exit /b 1
)
echo === vcpkg set to "%VCPKG_EXE%".

echo === installing vcpkg dependencies...
call "%VCPKG_EXE%" install --triplet=x64-windows --no-print-usage
if errorlevel 1 (
  echo [error] vcpkg install failed.
  exit /b 1
)
echo === vcpkg install succeeded.

echo === done.
pause
