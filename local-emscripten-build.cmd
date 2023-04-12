REM run as Administrator
@echo off

set DOWNLOADS_DIR=%USERPROFILE%\Downloads
set DOWNLOADS_DIR_LINUX=%DOWNLOADS_DIR:\=/%

@REM git submodule add https://github.com/emscripten-core/emsdk.git && cd emsdk && git checkout 3.1.25
SET EMSDK=%CD%\emsdk
SET EMSDK_NODE=%EMSDK%\node\14.18.2_64bit\bin\node.exe
SET EMSDK_PYTHON=%EMSDK%\python\3.9.2-nuget_64bit\python.exe
SET JAVA_HOME=%EMSDK%\java\8.152_64bit

SET PATH=^
%DOWNLOADS_DIR%\PortableGit\bin;^
%DOWNLOADS_DIR%\winlibs-x86_64-posix-seh-gcc-12.2.0-mingw-w64ucrt-10.0.0-r4\mingw64;^
%DOWNLOADS_DIR%\winlibs-x86_64-posix-seh-gcc-12.2.0-mingw-w64ucrt-10.0.0-r4\mingw64\bin;^
%DOWNLOADS_DIR%\cmake-3.22.2-windows-x86_64\bin;^
c:\openjdk-16\jdk-16\bin;^
%DOWNLOADS_DIR%\python-3.7.9-amd64-portable;^
%DOWNLOADS_DIR%\python-3.7.9-amd64-portable\Scripts;^
%EMSDK%;^
%EMSDK%\upstream\emscripten;^
%EMSDK%\node\14.18.2_64bit\bin;

@REM set PATH=^
@REM D:\Softwares\x86_64-8.1.0-release-win32-posix-rt_v6-rev0\mingw64;^
@REM D:\Softwares\x86_64-8.1.0-release-win32-posix-rt_v6-rev0\mingw64\bin;^
@REM D:\Softwares\cmake-3.23.0-rc1-windows-x86_64\bin;

set CD_LINUX=%CD:\=/%

echo %CD_LINUX%

cd %~dp0

if exist cmake-build rmdir /s /q cmake-build

@REM upgrade JAVA_HOME for antlr code generation
cd /d %EMSDK% &&^
.\emsdk install latest &&^
.\emsdk activate latest

set JAVA_HOME=c:\openjdk-16\jdk-16

cd /d "%~dp0" &&^
cmake.exe -G"MinGW Makefiles" ^
-DCMAKE_BUILD_TYPE=Debug ^
-DCMAKE_VERBOSE_MAKEFILE=ON ^
-DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%CD_LINUX%/build.emscripten" ^
-DCMAKE_TOOLCHAIN_FILE="%CD_LINUX%/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" ^
-Dantlr4-generator_DIR="%DOWNLOADS_DIR_LINUX%/antlr4-runtime-v4.11.1-emscripten-emsdk-3.1.25/lib/cmake/antlr4-generator" ^
-Dantlr4-runtime_DIR="%DOWNLOADS_DIR_LINUX%/antlr4-runtime-v4.11.1-emscripten-emsdk-3.1.25/lib/cmake/antlr4-runtime" ^
-B./cmake-build &&^
cd cmake-build &&^
cmake --build . &&^
echo "Successful build"
pause