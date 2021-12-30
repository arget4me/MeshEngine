@echo off

setlocal enabledelayedexpansion
call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\Auxiliary\\Build\\vcvarsall.bat" x64


set CPP_VERSION=c++latest

set EXE_NAME=MeshWindows
set EXE_NAME_DEBUG=%EXE_NAME%-DEBUG
set EXE_NAME_RELEASE=%EXE_NAME%-RELEASE

set LINK_DIRECTORIES=/LIBPATH:"..\\lib\\GLEW\\x64"
set LINK_LIBRARIES=user32.lib Gdi32.lib Opengl32.lib glew32s.lib

set SOURCE_PATH=..\\src\\
set SOURCE_FILES=%SOURCE_PATH%MeshEngine.cpp %SOURCE_PATH%Windows\\windows_platform_layer.cpp

set INCLUDE_DIRECTORIES=/I %SOURCE_PATH% /I ..\\include

set DEFINED_MACROS=-DUNICODE
set DEFINED_MACROS_DEBUG=%DEFINED_MACROS% -DDEBUG
set DEFINED_MACROS_RELEASE=%DEFINED_MACROS% -DRELEASE

mkdir ..\\..\\build
pushd ..\\..\\build


cl /Fe"%EXE_NAME_DEBUG%" %INCLUDE_DIRECTORIES% %DEFINED_MACROS_DEBUG% -Od -Zi -FC -EHsc -std:%CPP_VERSION% %SOURCE_FILES% -link %LINK_DIRECTORIES% %LINK_LIBRARIES% 
cl /Fe"%EXE_NAME_RELEASE%" %INCLUDE_DIRECTORIES% %DEFINED_MACROS_RELEASE% -FC -Oix -EHsc -std:%CPP_VERSION% %SOURCE_FILES% -link %LINK_DIRECTORIES% %LINK_LIBRARIES%
popd
endlocal