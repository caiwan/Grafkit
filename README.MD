# About

Yet another *demoscene engine/framework/spacechip firmware* or also refferrd by the fancy name ***rapid protoytping framework***, written in *c++11 or 14-17-ish flavour*, using *d3d11* optimized for VS2015. Full of junk code. Maintained by Caiwan/IR.

## Features
- modular code, with interfaces to extend it dynamically for your tase
- serialization and internal resouce management
- fair scenegraph support
- fairly easy to use - hack your own demos with it - or not
- small exporter tools and scripts for blender - we love blender isn!t it?

# Usage 
It finally uses Cmake to create its projects. For sample codes see tests folder and its subprojects. Cmake assumes to have your libraries in `../tools` folder relative to your cmake dir. It also uses small one-file libraries spread around the code.

# Depends on
- (Bass)[https://www.un4seen.com/] - Core audio
- (STB)[https://github.com/nothings/stb] - Core standard library

## Optional
- (Assimp)[https://github.com/assimp/assimp] - For native model import module
- (Bullet)[https://github.com/bulletphysics/bullet3] - for future usage 
- Lua - for future scripting engine
- Scintilla - for editor ui

## Tests
- (GoogleTest)[https://github.com/google/googletest]

# Future plans

- Make it work - and compatible partycoding
- Win Function
- Take world domination

## Development stuff

TBD
