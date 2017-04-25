file(GLOB_RECURSE FILES *.*)

add_custom_target(assets SOURCES ${FILES})
set_target_properties(assets PROPERTIES FOLDER tests)

# copy static sources 
foreach(_source IN ITEMS ${FILES})

	# this does not work for sme reason
	set_source_files_properties(${_source} PROPERTIES HEADER_FILE_ONLY TRUE)

	if (IS_ABSOLUTE "${_source}")
		file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
	else()
		set(source_rel "${_source}")
	endif()
	
	get_filename_component(_source_path "${_source_rel}" PATH)
	get_filename_component(_source_name "${_source_rel}" NAME)	
	set(dst_path "${CMAKE_CURRENT_BINARY_DIR}/${_source_path}")
	
	file(MAKE_DIRECTORY ${dst_path})
	
	# -- win32 only
	if (WIN32)
		string(REPLACE "/" "\\" _source "${_source}")
		string(REPLACE "/" "\\" _dest "${dst_path}/${_source_name}")
	else()
		message("nincs nem win32")
	endif(WIN32)
	
	add_custom_command(
		TARGET assets PRE_BUILD 
		COMMAND copy ${_source} ${_dest}
		DEPENDS ${_source}
	)

endforeach()

assign_source_group("${FILES}")

