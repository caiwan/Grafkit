@echo off

rem python -m pip install -r requirements.txt

REM if exist msvc rd /s /q msvc
if not exist msvc md msvc

cd msvc
cmake -DCMAKE_INSTALL_PREFIX=deploy/dev -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -G "Visual Studio 15 2017 Win64" ../ 
cd ..

rem python src\tools\sln_disable_shader_compil.py msvc/
