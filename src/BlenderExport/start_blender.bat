@echo off
blender --debug-memory -b -P exporter.py -- --input testdata/untitled.blend --output output/untitled.scene 
