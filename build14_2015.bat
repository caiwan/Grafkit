@echo off

call deleteCache.bat
call create14_2015.bat

cmake --build ./msvc --target install