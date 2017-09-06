@echo off
blender -b -P exporter.py -- --input animtest2.blend --test out.json
pause
