@echo off

set basedir=%CD%

cd ..\..\..\..\msvc\deploy\tools\

AssimpConverter.exe -b -lh -i %basedir%\scene1.blend -o %basedir%\..\scene1.scene
AssimpConverter.exe -b -lh -i %basedir%\scene2.blend -o %basedir%\..\scene2.scene
AssimpConverter.exe -b -lh -i %basedir%\scene3.blend -o %basedir%\..\scene3.scene

cd %basedir%
