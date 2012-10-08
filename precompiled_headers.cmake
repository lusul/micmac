#
# Christoph Heindl 2010
# Precompiled Headers Demo
# http://cheind.wordpress.com
#

# Instructs the MSVC toolset to use the precompiled header PRECOMPILED_HEADER
# for each source file given in the collection named by SOURCE_VARIABLE_NAME.
function(enable_precompiled_headers PRECOMPILED_HEADER SOURCE_VARIABLE_NAME TARGET_NAME)
if(WITH_HEADER_PRECOMP)
	if(MSVC_IDE)

		set(files ${${SOURCE_VARIABLE_NAME}})

		# Generate precompiled header translation unit
		get_filename_component(pch_basename ${PRECOMPILED_HEADER} NAME_WE)
		set(pch_abs ${PRECOMPILED_HEADER})
		set(pch_unity ${pch_basename}.cpp)
		FILE(WRITE ${pch_unity} "// Precompiled header unity generated by CMake\n")
		FILE(APPEND ${pch_unity} "#include <${pch_abs}>\n")
		set_source_files_properties(${pch_unity}  PROPERTIES COMPILE_FLAGS "/Yc\"${pch_abs}\"")

		# Update properties of source files to use the precompiled header.
		# Additionally, force the inclusion of the precompiled header at beginning of each source file.
		foreach(source_file ${files} )
		set_source_files_properties(
		${source_file} 
		PROPERTIES COMPILE_FLAGS
		"/Yu\"${pch_abs}\" /FI\"${pch_abs}\""
		)
		endforeach(source_file)

		# Finally, update the source file collection to contain the precompiled header translation unit
		set(${SOURCE_VARIABLE_NAME} ${${SOURCE_VARIABLE_NAME}} ${pch_unity} PARENT_SCOPE)
	endif(MSVC_IDE)

	IF(CMAKE_COMPILER_IS_GNUCXX)

		GET_FILENAME_COMPONENT(_name ${PRECOMPILED_HEADER} NAME)

		#SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${PRECOMPILED_HEADER}")
		SET(_source "${PROJECT_SOURCE_DIR}/include/${PRECOMPILED_HEADER}")
		#message("source : " ${_source})

		#SET(_outdir "${CMAKE_CURRENT_SOURCE_DIR}/${_name}.gch")
		SET(_outdir "${PROJECT_SOURCE_DIR}/include/${_name}.gch")
		MAKE_DIRECTORY(${_outdir})
		SET(_output "${_outdir}/.c++")

		STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
		SET(_compiler_FLAGS ${${_flags_var_name}})

		GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
		FOREACH(item ${_directory_flags})
		LIST(APPEND _compiler_FLAGS "-I${item}")
		ENDFOREACH(item)

		GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
		LIST(APPEND _compiler_FLAGS ${_directory_flags})

		SEPARATE_ARGUMENTS(_compiler_FLAGS)
		#MESSAGE("${CMAKE_CXX_COMPILER} -DPCHCOMPILE ${_compiler_FLAGS} -x c++-header -o ${_output} ${_source}")
		ADD_CUSTOM_COMMAND(
			OUTPUT ${_output}
			COMMAND ${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_source}
			DEPENDS ${_source} )
			ADD_CUSTOM_TARGET(${TARGET_NAME}_gch DEPENDS ${_output})

		ADD_DEPENDENCIES(${TARGET_NAME} ${TARGET_NAME}_gch)
		SET_TARGET_PROPERTIES(${TARGET_NAME}_gch PROPERTIES COMPILE_FLAGS "-include ${_name} -Winvalid-pch")

	ENDIF(CMAKE_COMPILER_IS_GNUCXX)
endif(WITH_HEADER_PRECOMP)
endfunction(enable_precompiled_headers)
