# About

Yet another framework for prototyping games and making fancy demoscene productions with it. It's written in *c++11 or 14-17-ish flavor*, using *d3d11* optimized for strictly VS2015 and VS2017, preconfigured with CMake.

**Due Direct3D only Windows and Visual Studio is supported** - despite usage of CMake. Extending with support of other compilers and IDEs are welcome.

## Features
- as a design goal making a modular code, with interfaces to extend it statically for your taste (e.g. does not support plug-in system ATM)
- serialization and internal resource management
- fair scene graph support
- fairly easy to use - desired to support bad architectural design to hack code with it - mean: party coding
- small exporter tools and scripts for Blender - we love Blender isn't it?

# Usage 
It finally uses Cmake to create its projects. For sample codes see `tests` folder and its sub projects. 

- Download all dependencies listed below, build and install them
- Add their install paths to `CMAKE_PREFIX_PATH` environment variable.
- Make sure that `cmake`, `python` and `blender` commands are available from `PATH` environment variable
- Run the corresponding `create_*.bat` script to your environment.

## Major dependencies
- Python 2.7
- Blender 2.79
- [Bass](https://www.un4seen.com/) - Core audio
- [STB](https://github.com/nothings/stb) - Core standard library
- [Assimp](https://github.com/assimp/assimp) - For native model import module
- [GoogleTest](https://github.com/google/googletest)
- [Scintilla](https://github.com/mirror/scintilla) - For editor :)

## Optional / Future
- (Bullet)[https://github.com/bulletphysics/bullet3] - for future usage 
- Lua - for future scripting engine

# Future plans

- Make it work
- Win Function
- Take world domination

## Issues

### Scinitlla non C++17 patch

As a quick and drity fix, for non C++17 compatible compilers, insert the following code after the include segment (line after `using namespace Scintilla;` ) of `Editor.cxx`, `ViewStyle.cxx` and `Document.cxx`.

```{.cpp}
namespace std {
	template<typename T> T clamp(T min, T max, T value) { return value > min ? (value < max ? value : max) : min; }
}
```

I will remove this later on.
