@echo off
setlocal
rem Configure and build with MSVC cl + Visual Studio's CMake and Ninja (no MSYS2).
rem Do not set VSINSTALLDIR before vcvars — that variable is used by vcvars64.bat.

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%i"
if not defined VSROOT (
  echo ERROR: Visual Studio with VC tools not found. Install "Desktop development with C++".
  exit /b 1
)

call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat"
set "PATH=%VSROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%VSROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"

cmake -S "%~dp0.." -B "%~dp0..\build-msvc" -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build "%~dp0..\build-msvc" --parallel
exit /b %ERRORLEVEL%
