# Declare static files - 
# caiwan/ir
function(copy_static_files TARGET INSTALL_DEST FILES)
	foreach(_file IN ITEMS ${FILES})

		if (IS_ABSOLUTE "${_file}")
			file(RELATIVE_PATH _file_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_file}")
		else()
			set(_file_rel "${_file}")
		endif()
		
		get_filename_component(_file_path "${_file_rel}" PATH)
		get_filename_component(_file_name "${_file_rel}" NAME)
		
		set(dst_path "${CMAKE_CURRENT_BINARY_DIR}/${_file_path}")
		
		file(MAKE_DIRECTORY ${dst_path})
		
		# -- win32 only
		if (WIN32)
			string(REPLACE "/" "\\" _file "${_file}")
			string(REPLACE "/" "\\" _dest "${dst_path}/${_file_name}")
		endif(WIN32)
		
		add_custom_command(
			TARGET ${TARGET} PRE_BUILD 
			COMMAND copy ${_file} ${_dest}
			DEPENDS ${_file}
		)
		
		message ("/{$_file_rel}")
		message ("${INSTALL_DEST}/{$_file_rel}")
		#install(
		#	FILES 
		#	"/{$_file_rel}"
		#	DESTINATION "${INSTALL_DEST}/{$_file_rel}"
		#)

	endforeach(_file)
endfunction(copy_static_files)
