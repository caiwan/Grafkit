# Creates a quick and dierty demo application with the engine
# copy-paste enginered with good care and love
# caiwan/IR

function(create_demoapp _application)
	file(GLOB_RECURSE SOURCE_FILES *.cpp *.c *.inl)
	file(GLOB_RECURSE HEADER_FILES *.h *.hpp *.inc)
    file(GLOB_RECURSE RC_FILES *.rc)
	
	include_directories("${CMAKE_SOURCE_DIR}")
	include_directories("${GK2_INCLUDE_DIR}")

	add_executable(${_application} ${SOURCE_FILES} ${HEADER_FILES} ${RC_FILES})

	target_link_libraries(${_application} Grafkit2)
	target_include_directories(${_application} PUBLIC include)

	set_target_properties(${_application} PROPERTIES FOLDER demos)

	msvc_set_win32(${_application})

	assign_source_group("${SOURCE_FILES}")
	assign_source_group("${HEADER_FILES}")
    assign_source_group("${RC_FILES}")

	target_link_libraries(${_application} ${BASS_LIBRARY_RELEASE})
	bass_copy_binaries(${_application} "${_application}")
	
	install(TARGETS ${_application} DESTINATION "${_application}")
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/bass.dll" DESTINATION "${_application}")
	
endfunction(create_demoapp)
