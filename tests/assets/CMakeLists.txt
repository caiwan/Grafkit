file(GLOB_RECURSE FILES *.*)

add_custom_target(assets-test EXCLUDE_FROM_ALL SOURCES ${FILES})
set_target_properties(assets-test PROPERTIES FOLDER tests)

copy_static_files(assets-test "tests/assets/" "${FILES}")

assign_source_group("${FILES}")
