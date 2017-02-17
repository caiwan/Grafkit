if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BASS_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(BASS_ARCHITECTURE "")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

set(BASS_ROOT_DIR CACHE PATH "BASS root directory")

find_path(BASS_INCLUDE_DIR NAMES bass.h   HINTS ${BASS_ROOT_DIR}/c/)
find_path(BASS_LIBRARY_DIR NAMES bass.lib HINTS ${BASS_ROOT_DIR}/c/${BASS_ARCHITECTURE}/)
find_path(BASS_DLL_DIR     NAMES bass.dll HINTS ${BASS_ROOT_DIR}/${BASS_ARCHITECTURE}/)

find_library(BASS_LIBRARY_RELEASE bass.lib  PATHS ${BASS_LIBRARY_DIR})

function(bass_copy_binaries TargetDir)
	file(COPY "${BASS_DLL_DIR}/bass.dll" DESTINATION ${TargetDir})
endfunction(bass_copy_binaries)

# FUNCTION(BASS_COPY_BINARIES TargetDirectory)
# 	ADD_CUSTOM_TARGET(BassCopyBinaries
# 		COMMAND ${CMAKE_COMMAND} -E copy honnan hova
# 		COMMAND ${CMAKE_COMMAND} -E copy honnan hova
# 	COMMENT "Copying Assimp binaries to '${TargetDirectory}'"
# 	VERBATIM)
# ENDFUNCTION(BASS_COPY_BINARIES)
