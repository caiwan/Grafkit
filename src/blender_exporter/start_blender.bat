@echo off
blender --debug-memory -b -P exporter.py -- --input anim_complex.blend --output anim_complex.scene 
