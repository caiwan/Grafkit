@echo off

@echo off
if exist msvc.build rd /s /q msvc.build
if not exist msvc.build md msvc.build

cd msvc.build
cmake -DCMAKE_INSTALL_PREFIX=deploy/live -DBUILD_LIVE_RELEASE=TRUE -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -G "Visual Studio 15 2017 Win64" ../ 
cd ..

python src\tools\sln_disable_shader_compil.py msvc/

cmake --build ./msvc.build --target install --config MinSizeRel

