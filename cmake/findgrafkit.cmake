# quinck and dirty was to fiund our own library
# to be used with embedding of other projects

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BASS_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(BASS_ARCHITECTURE "")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

set(GRAFKIT_ROOT_DIR CACHE PATH "GRAFKIT root directory")

find_path(GRAFKIT_INCLUDE_DIR NAMES common.h   HINTS ${GRAFKIT_ROOT_DIR}/inlcude/)
find_path(GRAFKIT_LIBRARY_DIR NAMES grafkit.lib HINTS ${GRAFKIT_ROOT_DIR}/lib/)

find_library(GRAFKIT_LIBRARY_RELEASE bass.lib PATHS ${GRAFKIT_ROOT_DIR})
find_library(GRAFKIT_LIBRARY_RELEASE bass.lib PATHS ${GRAFKIT_ROOT_DIR})

