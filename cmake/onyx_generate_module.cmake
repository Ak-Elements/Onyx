function(onyx_add_codegen_for_target)
    set(oneValueArgs
        TARGET
        RUNTIME_TARGET
        BASE_BINARY_DIR
        BASE_SOURCE_DIR
        GENERATED_DIR_SUFFIX
        IS_TOOLS_TARGET 
    )

    set(multiValueArgs
    )

    cmake_parse_arguments(arg "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    #### Pull data from target properties ####
    get_target_property(target_namespace        ${arg_TARGET} ONYX_NAMESPACE)
    get_target_property(target_ns_path          ${arg_TARGET} ONYX_NS_PATH)
    get_target_property(target_public_sources   ${arg_TARGET} ONYX_PUBLIC_SOURCES)
    get_target_property(target_public_deps      ${arg_TARGET} ONYX_PUBLIC_DEPENDENCIES)
    get_target_property(target_private_deps     ${arg_TARGET} ONYX_PRIVATE_DEPENDENCIES)
    get_target_property(target_type             ${arg_TARGET} TYPE)
    get_target_property(target_ocd_files        ${arg_TARGET} ONYX_OCD_FILES)

    #### Paths ####
    set(gen_root "${arg_BASE_BINARY_DIR}/${arg_GENERATED_DIR_SUFFIX}")

    set(public_gen_dir  "${gen_root}/public/${target_ns_path}")
    set(private_gen_dir "${gen_root}/private/${target_ns_path}")

    file(MAKE_DIRECTORY "${public_gen_dir}")
    file(MAKE_DIRECTORY "${private_gen_dir}")

    set(generated_public_sources "")
    set(generated_private_sources "")

    cmake_path(APPEND CMAKE_CURRENT_BINARY_DIR "tools" "${arg_GENERATED_DIR_SUFFIX}" "public" "${target_ns_path}" OUTPUT_VARIABLE tools_gen_public_dir)
    cmake_path(APPEND CMAKE_CURRENT_BINARY_DIR "tools" "${arg_GENERATED_DIR_SUFFIX}" "private" "${target_ns_path}" OUTPUT_VARIABLE tools_gen_private_dir)

    foreach(ocd_file IN LISTS target_ocd_files)
        cmake_path(RELATIVE_PATH ocd_file
            BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/public/${target_ns_path}"
            OUTPUT_VARIABLE relative_file_path
        )

        cmake_path(APPEND public_gen_dir  "${relative_file_path}" OUTPUT_VARIABLE gen_h)
        cmake_path(APPEND private_gen_dir "${relative_file_path}" OUTPUT_VARIABLE gen_cpp)

        cmake_path(REPLACE_EXTENSION gen_h   ".gen.h")
        cmake_path(REPLACE_EXTENSION gen_cpp ".gen.cpp")

        cmake_path(GET gen_h   PARENT_PATH pub_dir)
        cmake_path(GET gen_cpp PARENT_PATH priv_dir)

        file(MAKE_DIRECTORY "${pub_dir}")
        file(MAKE_DIRECTORY "${priv_dir}")

        list(APPEND generated_public_sources  "${gen_h}")
        list(APPEND generated_private_sources "${gen_cpp}")

        if (TARGET ${arg_TARGET}-tools)
            cmake_path(GET relative_file_path PARENT_PATH relative_file_path_dir)

            cmake_path(APPEND tools_gen_public_dir "${relative_file_path_dir}" OUTPUT_VARIABLE tools_gen_file_public_dir)
            cmake_path(APPEND tools_gen_private_dir "${relative_file_path_dir}" OUTPUT_VARIABLE tools_gen_file_private_dir)

            cmake_path(GET ocd_file STEM out_file_name)

            cmake_path(APPEND tools_gen_file_public_dir "${out_file_name}inspector.gen.h" OUTPUT_VARIABLE tools_gen_h)
            cmake_path(APPEND tools_gen_file_private_dir "${out_file_name}inspector.gen.cpp" OUTPUT_VARIABLE tools_gen_cpp)

            file(MAKE_DIRECTORY ${tools_gen_file_public_dir})
            file(MAKE_DIRECTORY ${tools_gen_file_private_dir})

            target_sources(${arg_TARGET}-tools PUBLIC
                FILE_SET HEADERS
                BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/tools/${arg_GENERATED_DIR_SUFFIX}/public"
                FILES ${tools_gen_h}
            )
            source_group(TREE ${tools_gen_public_dir} FILES ${tools_gen_h})

            target_sources(${arg_TARGET}-tools PRIVATE ${tools_gen_cpp})
            source_group(TREE ${tools_gen_private_dir} FILES ${tools_gen_cpp})

        endif()
    endforeach()

    #### Module init files ####
    set(module_header "${public_gen_dir}/${arg_TARGET}.gen.h")
    set(module_cpp "${private_gen_dir}/${arg_TARGET}.gen.cpp")

    list(APPEND generated_public_sources  "${module_header}")
    list(APPEND generated_private_sources "${module_cpp}")

    if (target_type STREQUAL "EXECUTABLE")
        set(init_cpp "${private_gen_dir}/init.gen.cpp")
        list(APPEND generated_private_sources "${init_cpp}")
    endif()

    #### Attach sources to target ####
    target_sources(${arg_TARGET} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${gen_root}/public"
        FILES ${generated_public_sources}
    )
    source_group(TREE ${public_gen_dir} FILES ${generated_public_sources})

    target_sources(${arg_TARGET} PRIVATE ${generated_private_sources})
    source_group(TREE ${private_gen_dir} FILES ${generated_private_sources})

    #### Codegen config ####
    build_include_list_for_target(
        include_dirs
        "${arg_TARGET}"
        "${target_public_deps}"
        "${target_private_deps}"
    )

    set(config "${gen_root}/codegen-config.toml")

    onyx_generate_codegen_config("${config}"
        TARGET ${arg_TARGET}
        RUNTIME_TARGET ${arg_RUNTIME_TARGET}
        NAMESPACE "${target_namespace}"
        TARGET_TYPE "${target_type}"
        IS_TOOLS_TARGET ${arg_IS_TOOLS_TARGET}
        INCLUDE_DIRS "${include_dirs}"
        PUBLIC_SOURCES "${target_public_sources}"
        GENERATED_DIR_SUFFIX "${arg_GENERATED_DIR_SUFFIX}"
        NAMESPACE_DIR_SUFFIX "${target_ns_path}"
    )

    #### Codegen command ####
    add_custom_command(
        OUTPUT ${generated_public_sources} ${generated_private_sources}
        COMMAND ${ONYX_CODEGEN} "${config}"
        DEPENDS
            "${target_public_sources}"
            "${ONYX_CODEGEN}"
            "${config}"
        COMMENT "Running Onyx codegen for ${arg_TARGET}"
        VERBATIM
    )
endfunction()

function(onyx_generate_codegen_config outPath)
    set(oneValueArgs
        TARGET
        RUNTIME_TARGET
        NAMESPACE
        TARGET_TYPE
        IS_TOOLS_TARGET
        GENERATED_DIR_SUFFIX
        NAMESPACE_DIR_SUFFIX
    )
    set(multiValueArgs
        PUBLIC_SOURCES
        INCLUDE_DIRS
    )

    cmake_parse_arguments(arg "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    toml_quote_list(public_sources_string ${arg_PUBLIC_SOURCES})
    toml_quote_list(include_directories_string ${arg_INCLUDE_DIRS}) # from write_all_includes_for_target logic

    set(toml_lines
        "[target]"
        "name = \"${arg_TARGET}\""
        "namespace = \"${arg_NAMESPACE}\""
        "is_executable = $<BOOL:$<STREQUAL:${arg_TARGET_TYPE},EXECUTABLE>>"
        "has_tools_target = $<TARGET_EXISTS:${arg_TARGET}-tools>"
        "is_tools_target = $<BOOL:${arg_IS_TOOLS_TARGET}>"
        "source_files = [${public_sources_string}]"
        "include_directories = [${include_directories_string}]"
        ""
    )

    set(target_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
    set(target_binary_dir ${CMAKE_CURRENT_BINARY_DIR})
    if (arg_IS_TOOLS_TARGET)
        set(target_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/tools")
        set(target_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/tools")
    endif()

    list(APPEND toml_lines
        "[paths]"
        "project_dir = \"${CMAKE_SOURCE_DIR}\""
        "source_dir = \"${target_source_dir}\""
        "binary_dir = \"${target_binary_dir}\""
        "dependencies_dir = \"$<IF:$<BOOL:${CPM_FETCHCONTENT_BASE_DIR}>,${CPM_FETCHCONTENT_BASE_DIR},${CMAKE_BINARY_DIR}/_deps>\""
        "generated_dir_suffix = \"${arg_GENERATED_DIR_SUFFIX}\""
        "namespace_dir_suffix = \"${arg_NAMESPACE_DIR_SUFFIX}\""
    )

    if (NOT arg_IS_TOOLS_TARGET AND TARGET ${arg_TARGET}-tools)
        list(APPEND toml_lines
            "tools_target_binary_dir = \"${CMAKE_CURRENT_BINARY_DIR}/tools\""
            ""
        )
    endif()

    string(JOIN "\n" toml_content ${toml_lines})

    file(GENERATE
        OUTPUT "${outPath}"
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
