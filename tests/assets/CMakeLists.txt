file(GLOB_RECURSE FILES *.*)

add_custom_target(assets SOURCES ${FILES})
set_target_properties(assets PROPERTIES FOLDER tests)

copy_static_files(assets "tests/assets/" "${FILES}")

assign_source_group("${FILES}")
