function(onyx_add_code_gen_target)
    set(options)
    set(oneValueArgs
        TARGET
        EDITOR_TARGET
        TARGET_TYPE
        NAMESPACE
        PUBLIC_DIR_SUFFIX
        PRIVATE_DIR_SUFFIX
        GENERATED_DIR_SUFFIX
        OUT_PUBLIC_SOURCES
        OUT_PRIVATE_SOURCES
        OUT_PUBLIC_EDITOR_SOURCES
        OUT_PRIVATE_EDITOR_SOURCES
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

    set(has_editor_target False)
    if (TARGET ${func_arg_EDITOR_TARGET})
        set(has_editor_target True)
    endif()

    set(is_executable False)
    if(func_arg_TARGET_TYPE STREQUAL "EXECUTABLE")
        set(is_executable True)
    endif()

    set(generated_public_sources)
    set(generated_private_sources)
    set(generated_public_editor_sources)
    set(generated_private_editor_sources)

    cmake_path(APPEND CMAKE_CURRENT_SOURCE_DIR ${func_arg_PUBLIC_DIR_SUFFIX} OUTPUT_VARIABLE target_base_include_directory)

    set(odslFiles ${func_arg_PUBLIC_SOURCES})
    list(FILTER odslFiles INCLUDE REGEX ".*\.${ONYX_COMPONENT_DEFINITION_FILE_EXTENSION}")
    foreach(odslFilePath IN LISTS odslFiles)

        cmake_path(RELATIVE_PATH odslFilePath BASE_DIRECTORY ${target_base_include_directory} OUTPUT_VARIABLE outRelativeOdslPath)
        
        cmake_path(APPEND target_public_binary_dir "${outRelativeOdslPath}" OUTPUT_VARIABLE odslGeneratedPublicPath)
        cmake_path(APPEND target_private_binary_dir "${outRelativeOdslPath}" OUTPUT_VARIABLE odslGeneratedPrivatePath)
               
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
        if (has_editor_target)

            cmake_path(APPEND target_editor_private_binary_dir "${outRelativeOdslPath}" OUTPUT_VARIABLE odslGeneratedEditorPrivatePath)
            cmake_path(GET odslGeneratedEditorPrivatePath PARENT_PATH editor_private_path_parent_directory)
            cmake_path(GET odslGeneratedEditorPrivatePath STEM out_file_name)
            cmake_path(APPEND editor_private_path_parent_directory "${out_file_name}_editor.gen.cpp" OUTPUT_VARIABLE editorGeneratedFilePath)
            
            file(MAKE_DIRECTORY ${editor_private_path_parent_directory})
            file(TOUCH ${editorGeneratedFilePath})

            list(APPEND generated_private_editor_sources "${editorGeneratedFilePath}")

        endif()

    endforeach()

    set(generated_module_header "${target_public_binary_dir}/${arg_TARGET_NAME}.gen.h")
    set_property(GLOBAL APPEND PROPERTY onyx_generated_module_headers "${generated_module_header}")

    if (is_executable)
        set(generated_init_path "${target_private_binary_dir}/init.gen.cpp")
        file(TOUCH ${generated_init_path})
        list(APPEND generated_private_sources ${generated_init_path}) 
    endif()

    list(APPEND generated_public_sources ${generated_module_header})
    list(APPEND generated_private_sources "${target_private_binary_dir}/${arg_TARGET_NAME}.gen.cpp")

    build_include_list_for_target(include_directories "${func_arg_TARGET}" "${func_arg_PUBLIC_DEPENDENCIES}" "${func_arg_PRIVATE_DEPENDENCIES}")
    generate_codegen_config()
    
    add_custom_command(
        OUTPUT ${generated_public_sources} ${generated_private_sources}
        COMMAND ${ONYX_CODEGEN} "${CMAKE_CURRENT_BINARY_DIR}/codegen-config.toml"
        DEPENDS
            ${func_arg_PUBLIC_SOURCES}      # If source changes, regenerate
            "${ONYX_CODEGEN}"               # generator changed
            "${CMAKE_CURRENT_BINARY_DIR}/codegen-config.toml" # generator config changed
        COMMENT "Running Onyx code generation for ${func_arg_TARGET}"
        VERBATIM
    )

    set(${func_arg_OUT_PUBLIC_SOURCES} ${generated_public_sources} PARENT_SCOPE)
    set(${func_arg_OUT_PRIVATE_SOURCES} ${generated_private_sources} PARENT_SCOPE)
    set(${func_arg_OUT_PUBLIC_EDITOR_SOURCES} ${generated_public_editor_sources} PARENT_SCOPE)
    set(${func_arg_OUT_PRIVATE_EDITOR_SOURCES} ${generated_private_editor_sources} PARENT_SCOPE)

endfunction()

function(generate_codegen_config)

    toml_quote_list(public_sources_string ${func_arg_PUBLIC_SOURCES})
    toml_quote_list(include_directories_string ${include_directories}) # from write_all_includes_for_target logic

    set(toml_lines
        "[target]"
        "name = \"${func_arg_TARGET}\""
        "namespace = \"${func_arg_NAMESPACE}\""
        "is_executable = $<BOOL:${is_executable}>"
        "has_editor_target = $<TARGET_EXISTS:${func_arg_EDITOR_TARGET}>"
        "source_files = [${public_sources_string}]"
        "include_directories = [${include_directories_string}]"
        ""
    )

    if (is_executable)
        get_property(onyx_generated_module_headers GLOBAL PROPERTY onyx_generated_module_headers)
        toml_quote_list(generated_module_headers_string ${onyx_generated_module_headers})

        list(APPEND toml_lines
        "generated_module_headers = [${generated_module_headers_string}]"
        )
    endif()

    list(APPEND toml_lines
        "[paths]"
        "project_dir = \"${CMAKE_SOURCE_DIR}\""
        "source_dir = \"${CMAKE_CURRENT_SOURCE_DIR}\""
        "binary_dir = \"${CMAKE_CURRENT_BINARY_DIR}\""
        "dependencies_dir = \"$<IF:$<BOOL:${CPM_FETCHCONTENT_BASE_DIR}>,${CPM_FETCHCONTENT_BASE_DIR},${CMAKE_BINARY_DIR}/_deps>\""
    )

    if(TARGET ${func_arg_EDITOR_TARGET})
        list(APPEND toml_lines
            "editor_binary_dir = \"${CMAKE_CURRENT_BINARY_DIR}/editor\""
        )
    endif()

    list(APPEND toml_lines
        "generated_dir_suffix = \"${func_arg_GENERATED_DIR_SUFFIX}\""
        "public_dir_suffix = \"${func_arg_PUBLIC_DIR_SUFFIX}\""
        "private_dir_suffix = \"${func_arg_PRIVATE_DIR_SUFFIX}\""
        ""
    )
    string(JOIN "\n" toml_content ${toml_lines})

    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/codegen-config.toml"
        CONTENT "${toml_content}"
    )

endfunction()

function(build_include_list_for_target OUT_INCLUDES TARGET PUBLIC_DEPS PRIVATE_DEPS)
    # Collect all deps (public + private + recursive)
    set(all_deps "${PUBLIC_DEPS};${PRIVATE_DEPS}")

    set(_deps ${PUBLIC_DEPS})
    list(APPEND _deps ${PRIVATE_DEPS})
    collect_recursive_deps(recursed "${_deps}")
    list(APPEND all_deps ${recursed})
    list(REMOVE_DUPLICATES all_deps)

    # Add the target itself first
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

    list(REMOVE_DUPLICATES all_raw)

    # Return result
    set(${OUT_INCLUDES} "${all_raw}" PARENT_SCOPE)
endfunction()

function(toml_quote_list out_var)
    set(result "")
    foreach(item IN LISTS ARGN)
        string(REPLACE "\\" "\\\\" item "${item}")
        string(REPLACE "\"" "\\\"" item "${item}")
        list(APPEND result "\"${item}\"")
    endforeach()
    list(JOIN result , result_string)
    set(${out_var} ${result_string} PARENT_SCOPE)
endfunction()

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
        if (NOT TARGET ${current})
            continue()
        endif()

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