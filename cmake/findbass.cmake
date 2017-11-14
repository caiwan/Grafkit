# quinck and dirty was to find bass library

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BASS_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(BASS_ARCHITECTURE "")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

path_find_in_list(BASS_ROOT_DIR "Bass24" ${CMAKE_PREFIX_PATH})

message("BASS_ROOT_DIR = ${BASS_ROOT_DIR}")

find_path(BASS_INCLUDE_DIR        NAMES  bass.h    PATHS ${BASS_ROOT_DIR}/c                       NO_DEFAULT_PATH)
find_path(BASS_DLL_DIR            NAMES  bass.dll  PATHS ${BASS_ROOT_DIR}/${BASS_ARCHITECTURE}    NO_DEFAULT_PATH)
find_library(BASS_LIBRARY_RELEASE        bass.lib  PATHS ${BASS_ROOT_DIR}/c/${BASS_ARCHITECTURE}  NO_DEFAULT_PATH)

# Copy bass to static && install dest
function(bass_copy_binaries TARGET_PROJECT TARGET_DIR)
	file(COPY "${BASS_DLL_DIR}/bass.dll" DESTINATION "${TARGET_DIR}")
endfunction(bass_copy_binaries)
