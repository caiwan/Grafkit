# Turn off compilation for shader files
# Caiwan/IR

function(msvc_nocompile_shader SHADER_FILE)
	if(MSVC)
		# set_source_files_properties(${SHADER_FILE} PROPERTIES COMPILE_FLAG )
	endif(MSVC)
endfunction(msvc_nocompile_shader)

