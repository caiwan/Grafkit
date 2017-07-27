@echo off

rem qnd converter script, will have cmake support later on
rem caiwan/IR

set basedir=%CD%

cd ..\..\msvc15\deploy\tools\

rem AssimpConverter.exe -b -lh -i %basedir%\models\locRotScale.blend -o %basedir%\locRotScale.scene
REM AssimpConverter.exe -b -lh -i %basedir%\models\sphere.blend -o %basedir%\sphere.scene
AssimpConverter.exe -b -lh -i %basedir%\models\spheres.blend -o %basedir%\spheres.scene
REM AssimpConverter.exe -b -lh -i %basedir%\models\box.blend -o %basedir%\box.scene

cd %basedir%
