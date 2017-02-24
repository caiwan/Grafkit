# ------------------------------------------------------------------------------------------

# http://stackoverflow.com/questions/13470499/cmake-add-custom-command-with-dependencies-from-a-different-directory

# Adds the files to the corresponding group bsed on their type and path, visual studio style
function(batch_configure_copy_file)
	foreach(_source IN ITEMS ${ARGN})

		if (IS_ABSOLUTE "${_source}")
			file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
		else()
			set(source_rel "${_source}")
		endif()
		
		get_filename_component(_source_path "${_source_rel}" PATH)
		
		set(dst_path "${CMAKE_CURRENT_BINARY_DIR}/${_source_path}")
		
		file(MAKE_DIRECTORY ${dst_path})
		
		# file(COPY "${_source_rel}" DESTINATION ${dst_path})
		add_custom_command(
			OUTPUT "${_source_rel}"
			COMMAND ${CMAKE_COMMAND} -E copy ${_source_rel} ${dst_path}
			DEPENDS ${_source_rel}
			)

	endforeach()
endfunction(batch_configure_copy_file)

# ------------------------------------------------------------------------------------------
