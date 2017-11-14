file(GLOB_RECURSE FILES *.*)

add_custom_target(assets-test SOURCES ${FILES})
set_target_properties(assets-test PROPERTIES FOLDER tests)
# set_target_properties(assets-test PROPERTIES EXCLUDE_FROM_ALL TRUE)

bass_copy_binaries(assets-test "${CMAKE_CURRENT_BINARY_DIR}")

copy_static_files(assets-test "tests/assets/" "${FILES}")

assign_source_group("${FILES}")
