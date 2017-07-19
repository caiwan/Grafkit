@echo off

set basedir=%CD%

cd ..\..\msvc\deploy\tools\

AssimpConverter.exe -b -lh -i %basedir%\models\locRotScale.blend -o %basedir%\locRotScale.scene
rem AssimpConverter.exe -b -lh -i %basedir%\models\scene2.blend -o %basedir%\scene2.scene
rem AssimpConverter.exe -b -lh -i %basedir%\models\scene3.blend -o %basedir%\scene3.scene

cd %basedir%
