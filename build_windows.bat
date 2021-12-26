@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set EXE_NAME=MeshWindows
set EXE_NAME_DEBUG=%EXE_NAME%-DEBUG
set EXE_NAME_RELEASE=%EXE_NAME%-RELEASE

set LINK_DIRECTORIES=/LIBPATH:"../lib/GLEW/x64"
set LINK_LIBRARIES=user32.lib Gdi32.lib Opengl32.lib glew32s.lib

set SOURCE_PATH=../src/
set SOURCE_FILES=%SOURCE_PATH%Windows/windows_platform_layer.cpp %SOURCE_PATH%MeshEngine.cpp

set INCLUDE_DIRECTORIES=/I %SOURCE_PATH% /I ../include

set DEFINED_MACROS=

mkdir build
pushd build
cl /Fe"%EXE_NAME_DEBUG%" %INCLUDE_DIRECTORIES% %DEFINED_MACROS% -Od -Zi -FC %SOURCE_FILES% -link %LINK_DIRECTORIES% %LINK_LIBRARIES% 
cl /Fe"%EXE_NAME_RELEASE%" %INCLUDE_DIRECTORIES% %DEFINED_MACROS% -FC -Oix %SOURCE_FILES% -link %LINK_DIRECTORIES% %LINK_LIBRARIES%
popd