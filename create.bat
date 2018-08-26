@echo off

if not exist msvc md msvc

cd msvc
cmake -DCMAKE_INSTALL_PREFIX=deploy/dev -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -DCMAKE_GENERATOR_PLATFORM=x64 ../ 
cd ..
