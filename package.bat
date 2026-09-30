@echo off
setlocal

set "ROOT=%~dp0"
set "RELEASE_DIR=%ROOT%bin\Release-x86_64\Editor"
set "PACKAGE_DIR=%ROOT%package"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

echo === checking release build...
if not exist "%RELEASE_DIR%\Editor.exe" (
  echo [error] "%RELEASE_DIR%\Editor.exe" not found, build release first.
  exit /b 1
)
echo === release build found.

echo === reading version...
for /f tokens^=2^ delims^=^" %%v in ('findstr /c:"kVersion" "%ROOT%seri\seri\core\Literals.h"') do set "VERSION=%%v"
if not defined VERSION (
  echo [error] version not found.
  exit /b 1
)
set "PACKAGE_NAME=seri-game-engine-x64-v%VERSION%"
set "STAGE_DIR=%PACKAGE_DIR%\%PACKAGE_NAME%"
set "ZIP_FILE=%PACKAGE_DIR%\%PACKAGE_NAME%.zip"
echo === version is "%VERSION%".

echo === finding visual studio runtimes...
if not exist "%VSWHERE%" (
  echo [error] vswhere not found.
  exit /b 1
)
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_DIR=%%i"
if not defined VS_DIR (
  echo [error] visual studio with c++ tools not found.
  exit /b 1
)
set /p REDIST_VERSION=<"%VS_DIR%\VC\Auxiliary\Build\Microsoft.VCRedistVersion.default.txt"
for /d %%d in ("%VS_DIR%\VC\Redist\MSVC\%REDIST_VERSION%\x64\Microsoft.VC*.CRT") do set "CRT_DIR=%%d"
if not defined CRT_DIR (
  echo [error] runtime folder not found for redist version "%REDIST_VERSION%".
  exit /b 1
)
echo === runtimes found at "%CRT_DIR%".

echo === preparing package folder...
if exist "%STAGE_DIR%" rmdir /s /q "%STAGE_DIR%"
if exist "%STAGE_DIR%" (
  echo [error] could not clean "%STAGE_DIR%", is a file in it open?
  exit /b 1
)
if exist "%ZIP_FILE%" del /q "%ZIP_FILE%"
if exist "%ZIP_FILE%" (
  echo [error] could not replace "%ZIP_FILE%", is it open?
  exit /b 1
)
mkdir "%STAGE_DIR%"
echo === package folder ready.

echo === copying release files...
copy /y "%RELEASE_DIR%\Editor.exe" "%STAGE_DIR%\" >nul
if errorlevel 1 (
  echo [error] could not copy Editor.exe.
  exit /b 1
)
copy /y "%RELEASE_DIR%\*.dll" "%STAGE_DIR%\" >nul
if errorlevel 1 (
  echo [error] could not copy dlls.
  exit /b 1
)
xcopy /e /i /q /y "%RELEASE_DIR%\assets" "%STAGE_DIR%\assets" >nul
if errorlevel 1 (
  echo [error] could not copy assets.
  exit /b 1
)
echo === release files copied.

echo === copying runtimes...
for %%f in (msvcp140.dll vcruntime140.dll vcruntime140_1.dll) do (
  copy /y "%CRT_DIR%\%%f" "%STAGE_DIR%\" >nul
  if errorlevel 1 (
    echo [error] could not copy %%f.
    exit /b 1
  )
)
echo === runtimes copied.

echo === zipping...
tar -a -c -f "%ZIP_FILE%" -C "%STAGE_DIR%" *
if errorlevel 1 (
  echo [error] zip failed.
  exit /b 1
)
echo === zipped to "%ZIP_FILE%".

echo === cleaning copied files...
rmdir /s /q "%STAGE_DIR%"
if exist "%STAGE_DIR%" (
  echo [error] could not delete "%STAGE_DIR%".
  exit /b 1
)
echo === copied files cleaned.

echo === done.
pause
