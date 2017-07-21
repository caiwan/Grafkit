@echo off

rem qnd converter script, will have cmake support later on
rem caiwan/IR

set basedir=%CD%

cd ..\..\msvc\deploy\tools\

AssimpConverter.exe -b -lh -i %basedir%\models\locRotScale.blend -o %basedir%\locRotScale.scene
AssimpConverter.exe -b -lh -i %basedir%\models\sphere.blend -o %basedir%\sphere.scene
rem AssimpConverter.exe -b -lh -i %basedir%\models\scene3.blend -o %basedir%\scene3.scene

cd %basedir%
