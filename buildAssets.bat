@echo off
rem qnd batch compiler

set basedir=%CD%
set convcwd=%CD%\msvc\deploy\tools\
set convcmd=%convcwd%\AssimpConverter.exe
pushd tests\assets
for /r %%i in (*.blend) do (
    %convcmd% -b --ln -i %%i -o %CD%\%%~ni.scene --cwd %convcwd%
)
popd