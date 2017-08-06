file(GLOB_RECURSE FILES *.*)

add_custom_target(test-assets SOURCES ${FILES})
set_target_properties(test-assets PROPERTIES FOLDER tests)

copy_static_files(test-assets "tests/assets/" "${FILES}")

assign_source_group("${FILES}")
