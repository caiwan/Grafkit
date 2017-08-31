@echo off

rem qnd converter script, will have cmake support later on
rem caiwan/IR

set basedir=%CD%

cd ..\..\msvc\deploy\tools\

AssimpConverter.exe -b --lh -i %basedir%\models\animtest1.blend -o %basedir%\animtest1.scene
AssimpConverter.exe -b --lh -i %basedir%\models\animtest2.blend -o %basedir%\animtest2.scene

AssimpConverter.exe -b --lh -i %basedir%\models\locRotCamera.blend -o %basedir%\locRotCamera.scene
AssimpConverter.exe -b --lh -i %basedir%\models\locRotScale.blend -o %basedir%\locRotScale.scene
AssimpConverter.exe -b --lh -i %basedir%\models\sphere.blend -o %basedir%\sphere.scene
AssimpConverter.exe -b --lh -i %basedir%\models\spheres.blend -o %basedir%\spheres.scene
AssimpConverter.exe -b --lh -i %basedir%\models\box.blend -o %basedir%\box.scene
AssimpConverter.exe -b --lh -i %basedir%\models\sphere_multimaterial.blend -o %basedir%\sphere_multimaterial.scene
AssimpConverter.exe -b --lh -i %basedir%\models\ao.blend -o %basedir%\ao.scene

cd %basedir%
