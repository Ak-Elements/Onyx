function(onyx_add_code_gen_target)
    set(options)
    set(oneValueArgs
        TARGET
        NAMESPACE
        PUBLIC_BINARY_DIR
        PRIVATE_BINARY_DIR
        EDITOR_PRIVATE_BINARY_DIR
        OUT_PUBLIC_SOURCES
        OUT_PRIVATE_SOURCES
    )
    set(multiValueArgs
        PUBLIC_SOURCES
        PRIVATE_SOURCES
        PUBLIC_DEPENDENCIES
        PRIVATE_DEPENDENCIES
    )

    cmake_parse_arguments(
        func_arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    set(generated_public_sources)
    set(generated_private_sources)

    # Temporary input lists for the code generator
    set(SRC_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/sourcefiles")
    set(INC_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/includedirectories")

    write_all_sources("${SRC_LIST_FILE}" "${func_arg_PUBLIC_SOURCES}")
    write_all_includes_for_target(
        ${func_arg_TARGET}
        "${INC_LIST_FILE}"
        "${func_arg_PUBLIC_DEPENDENCIES}"
        "${func_arg_PRIVATE_DEPENDENCIES}"
    )

    set(code_gen_args
        module
        --target "${func_arg_TARGET}"
        --namespace "${func_arg_NAMESPACE}"
        --source-dir "${CMAKE_CURRENT_SOURCE_DIR}"
        --binary-dir "${CMAKE_CURRENT_BINARY_DIR}"
        --public-dir "${func_arg_PUBLIC_BINARY_DIR}"
        --private-dir "${func_arg_PRIVATE_BINARY_DIR}"
    )

    cmake_path(IS_PREFIX onyx_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} is_onyx_module)
    if (is_onyx_module)
        list(APPEND code_gen_args
            --editor-dir "${func_arg_EDITOR_PRIVATE_BINARY_DIR}"
        )
    endif()

    # make generated source folders
    file(MAKE_DIRECTORY ${func_arg_PUBLIC_DIR})
    file(MAKE_DIRECTORY ${func_arg_PRIVATE_DIR})

    set(odslFiles ${func_arg_PUBLIC_SOURCES})
    list(FILTER odslFiles INCLUDE REGEX ".*\.${ONYX_COMPONENT_DEFINITION_FILE_EXTENSION}")
    foreach(odslFilePath IN LISTS odslFiles)

        cmake_path(RELATIVE_PATH odslFilePath BASE_DIRECTORY ${arg_PUBLIC_SOURCES_DIR} OUTPUT_VARIABLE outRelativeOdslPath)
        
        cmake_path(APPEND arg_PUBLIC_BINARY_DIR "${outRelativeOdslPath}" OUTPUT_VARIABLE odslGeneratedPublicPath)
        cmake_path(APPEND arg_PRIVATE_BINARY_DIR "${outRelativeOdslPath}" OUTPUT_VARIABLE odslGeneratedPrivatePath)
               
        cmake_path(REPLACE_EXTENSION odslGeneratedPublicPath ".gen.h")
        cmake_path(REPLACE_EXTENSION odslGeneratedPrivatePath ".gen.cpp")
        
        cmake_path(GET odslGeneratedPublicPath PARENT_PATH public_path_parent_directory)
        cmake_path(GET odslGeneratedPrivatePath PARENT_PATH private_path_parent_directory)

        file(MAKE_DIRECTORY ${public_path_parent_directory})
        file(MAKE_DIRECTORY ${private_path_parent_directory})

        file(TOUCH ${odslGeneratedPublicPath})
        file(TOUCH ${odslGeneratedPrivatePath})

        list(APPEND generated_public_sources ${odslGeneratedPublicPath})
        list(APPEND generated_private_sources ${odslGeneratedPrivatePath})
        
        # add editor
        if (is_onyx_module)
            cmake_path(APPEND func_arg_EDITOR_PRIVATE_BINARY_DIR "${outRelativeOdslPath}" OUTPUT_VARIABLE odslGeneratedEditorPrivatePath)
            cmake_path(GET odslGeneratedEditorPrivatePath PARENT_PATH editor_private_path_parent_directory)
            cmake_path(GET odslGeneratedEditorPrivatePath STEM out_file_name)
            cmake_path(APPEND editor_private_path_parent_directory "${out_file_name}_editor.gen.cpp" OUTPUT_VARIABLE editorGeneratedFilePath)
            
            file(MAKE_DIRECTORY ${editor_private_path_parent_directory})
            file(TOUCH ${editorGeneratedFilePath})
       
            get_property(editor_implementations GLOBAL PROPERTY onyx_generated_editor_implementations)
            list(APPEND editor_implementations "${editorGeneratedFilePath}")
            set_property(GLOBAL PROPERTY onyx_generated_editor_implementations "${editor_implementations}")

        endif()

    endforeach()

    set(generated_module_header "${arg_PUBLIC_BINARY_DIR}/${arg_TARGET_NAME}.gen.h")
    set_property(GLOBAL APPEND PROPERTY onyx_generated_module_headers "${generated_module_header}")

    list(APPEND generated_public_sources ${generated_module_header})
    list(APPEND generated_private_sources "${arg_PRIVATE_BINARY_DIR}/${arg_TARGET_NAME}.gen.cpp")

    add_custom_command(
        OUTPUT ${generated_public_sources} ${generated_private_sources}
        COMMAND ${ONYX_CODEGEN} ${code_gen_args}
        DEPENDS
            ${func_arg_PUBLIC_SOURCES}      # If source changes, regenerate
            "${ONYX_CODEGEN}"               # generator changed
            "${INC_LIST_FILE}"              # includes changed
        COMMENT "Running Onyx code generation for ${func_arg_TARGET}"
        VERBATIM
    )



    set(${func_arg_OUT_PUBLIC_SOURCES} ${generated_public_sources} PARENT_SCOPE)
    set(${func_arg_OUT_PRIVATE_SOURCES} ${generated_private_sources} PARENT_SCOPE)
endfunction()

function(write_all_includes_for_target TARGET FILE PUBLIC_DEPS PRIVATE_DEPS)
    # This function writes includes of:
    # - TARGET
    # - PUBLIC_DEPS list
    # - PRIVATE_DEPS list
    # into FILE

    # Work in a temporary file list
    set(all_raw "")

    # Collect all deps including recursive ones
    set(all_deps "${PUBLIC_DEPS};${PRIVATE_DEPS}")

    set(_deps ${PUBLIC_DEPS})
    list(APPEND _deps ${PRIVATE_DEPS})
    collect_recursive_deps(recursed "${_deps}")
    list(APPEND all_deps ${recursed})
    list(REMOVE_DUPLICATES all_deps)
    
    # Add the target itself at the front
    set(all_targets "${TARGET};${all_deps}")
   
    set(all_raw "")

    foreach(tgt IN LISTS all_targets)
        if(NOT TARGET "${tgt}")
            continue()
        endif()

        get_target_property(incs "${tgt}" INTERFACE_INCLUDE_DIRECTORIES)
        if(incs)
            list(APPEND all_raw ${incs})
        endif()
    endforeach()

    # remove include directories outside of the engine / project 
    string(REPLACE "/" "\/" regex_pattern "${CPM_FETCHCONTENT_BASE_DIR}")
    list(FILTER all_raw EXCLUDE REGEX "${regex_pattern}.*")

    # Deduplicate
    list(REMOVE_DUPLICATES all_raw)

    # Join with newlines for file(GENERATE)
    string(REPLACE ";" "\n" merged "${all_raw}")

    file(GENERATE
        OUTPUT "${FILE}"
        CONTENT "${merged}"
    )

endfunction()

function(write_all_sources FILE PUBLIC_SOURCES)
    # Convert to newline-separated output
    string(REPLACE ";" "\n" text "${PUBLIC_SOURCES}")
    file(WRITE "${FILE}" "${text}")
endfunction()

# Collect all recursive linked targets of a target
function(collect_recursive_deps RESULT TARGETS)
    set(visited "")
    set(queue "${TARGETS}")

    while(queue)
        list(POP_FRONT queue current)

        if(current IN_LIST visited)
            continue()
        endif()
        list(APPEND visited "${current}")

        # Fetch its linked targets
        get_target_property(libs "${current}" INTERFACE_LINK_LIBRARIES)
        if(libs)
            foreach(lib IN LISTS libs)
                if(TARGET "${lib}" AND NOT lib IN_LIST visited)
                    list(APPEND queue "${lib}")
                endif()
            endforeach()
        endif()

    endwhile()

    # Remove original target from result
    list(REMOVE_ITEM visited "${TARGET}")

    set(${RESULT} "${visited}" PARENT_SCOPE)
endfunction()