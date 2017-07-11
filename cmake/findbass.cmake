# quinck and dirty was to fiund bass library

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BASS_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(BASS_ARCHITECTURE "")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

set(BASS_ROOT_DIR CACHE PATH "BASS root directory")

find_path(BASS_INCLUDE_DIR NAMES bass.h   HINTS ${BASS_ROOT_DIR}/c/)
find_path(BASS_LIBRARY_DIR NAMES bass.lib HINTS ${BASS_ROOT_DIR}/c/${BASS_ARCHITECTURE}/)
find_path(BASS_DLL_DIR     NAMES bass.dll HINTS ${BASS_ROOT_DIR}/${BASS_ARCHITECTURE}/)

find_library(BASS_LIBRARY_RELEASE bass.lib PATHS ${BASS_LIBRARY_DIR})

# Copy bass to static && install dest
function(bass_copy_binaries TARGET_PROJECT TARGET_DIR)
	# can't copy 
	# string(REPLACE "/" "\\" ASSIMP_DLL_BASEDIR "${ASSIMP_ROOT_DIR}/bin")
	# string(REPLACE "/" "\\" TARGET_DIR "${TargetDirectory}")
	# 		
	# add_custom_command(TARGET ${TARGET_PROJECT} COMMAND copy "${BASS_DLL_DIR}/bass.dll" "${TARGET_DIR}/bass.dll")
endfunction(bass_copy_binaries)

