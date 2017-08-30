@echo off

call deleteCache.bat

cd msvc
cmake -DBUILD_LIVE_RELEASE=TRUE -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -G "Visual Studio 15 2017 Win64" ../ 
cd ..

python src\tools\sln_disable_shader_compil.py msvc/

cmake --build ./msvc --target install