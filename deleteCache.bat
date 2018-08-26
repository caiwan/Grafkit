@echo off
rem if exist msvc rd /s /q msvc
if exist msvc\CMakeCache.txt del msvc\CMakeCache.txt
if not exist msvc md msvc