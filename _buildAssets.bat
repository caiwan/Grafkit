@echo off
rem qnd batch compiler

if not exist msvc.build goto please_build

rem find all blends and convert them
set basedir=%CD%
set convcwd=%CD%\msvc.build\deploy\tools\
set convcmd=%convcwd%\AssimpConverter.exe
pushd tests\assets
for /r %%i in (*.blend) do (
    %convcmd% -b --ln -i %%i -o %CD%\%%~ni.scene --cwd %convcwd%
)
popd

goto end

:please_build
echo Please build production code first
pause

:end
