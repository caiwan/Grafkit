file(GLOB_RECURSE ASSET_FILES *.*)

add_custom_target(assets-test SOURCES ${ASSET_FILES})
set_target_properties(assets-test PROPERTIES FOLDER tests)

set_source_files_properties(${ASSET_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")

bass_copy_binaries(assets-test "${CMAKE_CURRENT_BINARY_DIR}")

copy_static_files(assets-test "tests/assets/" "${ASSET_FILES}")

assign_source_group("${ASSET_FILES}")
