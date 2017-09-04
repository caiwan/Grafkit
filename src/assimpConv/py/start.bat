@echo off
blender -b -P exporter.py -- --input sphere_pbr.blend --test out.json
pause
