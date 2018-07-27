file(GLOB_RECURSE TEXTURE_FILES textures/*.*)
file(GLOB_RECURSE MODEL_FILES models/*.*)
file(GLOB_RECURSE SHADER_FILES shaders/*.*)
file(GLOB_RECURSE JSON_FILES *.json)
file(GLOB_RECURSE MUSIC_FILES *.mp3 *.ogg)

add_custom_target(assets-test SOURCES ${TEXTURE_FILES} ${SHADER_FILES} ${JSON_FILES} ${MUSIC_FILES})
set_target_properties(assets-test PROPERTIES FOLDER tests)

set_source_files_properties(${TEXTURE_FILES} ${SHADER_FILES} ${JSON_FILES} ${MUSIC_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

bass_copy_binaries(assets-test "${CMAKE_CURRENT_BINARY_DIR}")
copy_static_files(assets-test "tests/assets/textures/" "${TEXTURE_FILES}")
copy_static_files(assets-test "tests/assets/textures/" "${TEXTURE_FILES}")
copy_static_files(assets-test "tests/assets/shaders/" "${SHADER_FILES}")
copy_static_files(assets-test "tests/assets/" "${JSON_FILES}")
copy_static_files(assets-test "tests/assets/" "${MUSIC_FILES}")

assign_source_group(${TEXTURE_FILES} ${SHADER_FILES} ${JSON_FILES} ${MUSIC_FILES})
