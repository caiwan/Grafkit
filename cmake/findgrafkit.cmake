# quinck and dirty was to fiund our own library
# to be used with embedding of other projects

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BASS_ARCHITECTURE "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(BASS_ARCHITECTURE "")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

# this simply does not work at all
#if (NOT GK_ROOT_DIR)
# path_find_in_list(GK_ROOT_DIR "grafkit2" ${CMAKE_PREFIX_PATH})
#endif()

find_library(GRAFKIT_LIBRARY_RELEASE grafkit2.lib HINTS ${GK_ROOT_DIR}/lib ${GK_ROOT_DIR}/msvc/deploy/lib)
find_library(GRAFKIT_LIBRARY_DEBUG grafkit2-d.lib HINTS ${GK_ROOT_DIR}/lib ${GK_ROOT_DIR}/msvc/deploy/lib)

message("GRAFKIT_LIBRARY_RELEASE ${GRAFKIT_LIBRARY_RELEASE}" )
get_filename_component(GK_ROOT_DIR ${GRAFKIT_LIBRARY_RELEASE} PATH)
get_filename_component(GK_ROOT_DIR ${GK_ROOT_DIR}/../ ABSOLUTE)
message("GK_ROOT_DIR ${GK_ROOT_DIR}" )

find_path(GRAFKIT_INCLUDE_DIR NAMES common.h stdafx.h HINTS ${GK_ROOT_DIR}/inlcude)



find_path(BLENDERCONV_DIR NAMES AssimpConverter.exe PATHS ${GRAFKIT_INCLUDE_DIR}/../tools NO_DEFAULT_PATH)

get_filename_component(BLENDERCONV_DIR ${BLENDERCONV_DIR} ABSOLUTE)


set(BLENDERCONV_COMMAND ${BLENDERCONV_DIR}/AssimpConverter.exe)

link_directories(${GRAFKIT_LIBRARY_RELEASE})
