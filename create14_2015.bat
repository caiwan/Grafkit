@echo off
    
REM if exist msvc rd /s /q msvc
if not exist msvc md msvc

cd msvc
cmake -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -G "Visual Studio 14 2015 Win64" ../
cd ..

python src\tools\sln_disable_shader_compil.py -i msvc/
