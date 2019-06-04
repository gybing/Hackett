CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

# 启用目录
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

if(HCMAKE_DEFINE_INCLUDE)
  return()
endif()
set(HCMAKE_DEFINE_INCLUDE TRUE)

###
# 设置输出目录
# 设置目标输出属性，包括命名和目录规范
# 命名规范：静态库加lib前缀
# 输出目录规范：${OUTPUT}/${CMAKE_PLATFORM_OUT}/$<CONFIG>
# 支持第3个参数(可选)指定和TARGET不同的输出名称
# windows平台下的导入库输出由FUNC_TARGET_IMPLIB函数设置
function(FUNC_OUTPUT TARGET OUTPUT)
	# 第3个参数是输出名称
	if(${ARGC} GREATER 2)
		set(NAME ${ARGV2})
		string(STRIP ${NAME} NAME)
	else()
		set(NAME ${TARGET})
	endif()

	# 设置平台子目录
	set(OUTPUT_DIRECTORY ${OUTPUT}/${CMAKE_PLATFORM_OUT})

	# 取目标类型
	get_target_property(TARGET_TYPE ${TARGET} TYPE)

	# 根据目标类型设置属性
	# 静态库总是输出到ARCHIVE，设置输出名称使vs和Unix保持一致，并且和导入库命名(无lib前缀)区分开
	if(${TARGET_TYPE} STREQUAL "STATIC_LIBRARY")
		# Windows平台需要设置PDB输出目录，lib前缀
		if(WINDOWS)
			set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_NAME lib${NAME})
			set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/$<CONFIG>)
	
			# COMPILE_PDB_OUTPUT_DIRECTORY不支持Generator Expression
			set_target_properties(${TARGET} PROPERTIES COMPILE_PDB_NAME lib${NAME})			
			set_target_properties(${TARGET} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/Debug)
			set_target_properties(${TARGET} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY}/Release)
			set_target_properties(${TARGET} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG_STATICCRT ${OUTPUT_DIRECTORY}/Debug_StaticCRT)
			set_target_properties(${TARGET} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY_RELEASE_STATICCRT ${OUTPUT_DIRECTORY}/Release_StaticCRT)
		else()
			set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_NAME ${NAME})
			set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/$<CONFIG>)
		endif()
	# 共享库和执行程序的输出名称和目录与系统相关，windows下均为RUNTIME，Unix下共享库为LIBRARY，执行程序为RUNTIME
	# 其实可以只设置OUTPUT_NAME，但是OUTPUT_DRIECTORY还是需要分别设置
	elseif(${TARGET_TYPE} STREQUAL "SHARED_LIBRARY" OR ${TARGET_TYPE} STREQUAL "EXECUTABLE")
		# Windows平台下DLL/EXE均为RUNTIME，导入库输出目录需要单独设置(FUNC_TARGET_IMPLIB)
		if(WINDOWS)
			set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_NAME ${NAME})
			set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/$<CONFIG>)

			# 需要设置PDB名称和输出目录
			set_target_properties(${TARGET} PROPERTIES PDB_NAME ${NAME})
			# PDB_OUTPUT_DIRECTORY不支持Generator Expression
			set_target_properties(${TARGET} PROPERTIES PDB_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/Debug)
			set_target_properties(${TARGET} PROPERTIES PDB_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY}/Release)
			set_target_properties(${TARGET} PROPERTIES PDB_OUTPUT_DIRECTORY_DEBUG_STATICCRT ${OUTPUT_DIRECTORY}/Debug_StaticCRT)
			set_target_properties(${TARGET} PROPERTIES PDB_OUTPUT_DIRECTORY_RELEASE_STATICCRT ${OUTPUT_DIRECTORY}/Release_StaticCRT)
		# 非Windows平台下的共享库需要设置LIBRARY_OUTPUT
		elseif(${TARGET_TYPE} STREQUAL "SHARED_LIBRARY")
			set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_NAME ${NAME})
			set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/$<CONFIG>)
		# 非Windows平台下的执行程序需要设置RUNTIME_OUTPUT
		else()
			set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_NAME ${NAME})
			set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/$<CONFIG>)
		endif()
	else()
		message(STATUS "No output property set for target ${TARGET} of type ${TARGET_TYPE}")
	endif()

endfunction(FUNC_OUTPUT)

# 文件分组
function(ASSIGN_SOURCE_GROUP)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(ASSIGN_SOURCE_GROUP)
