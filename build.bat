@echo off

call create.bat

cmake --build ./msvc --target install --config MinSizeRel