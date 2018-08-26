@echo off

call deleteCache.bat

cd msvc
cmake -DCMAKE_INSTALL_PREFIX=deploy/live -DBUILD_LIVE_RELEASE=TRUE -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -G "Visual Studio 14 2015 Win64" ../ 
cd ..

cmake --build ./msvc --target install