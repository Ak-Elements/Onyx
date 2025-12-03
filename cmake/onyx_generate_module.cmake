function(onyx_add_code_gen_target TARGET TARGET_NAMESPACE GENERATED_PUBLIC_PATH GENERATED_PRIVATE_PATH GENERATED_HEADER_PATH GENERATED_CPP_PATH PUBLIC_SOURCES PUBLIC_DEPS PRIVATE_DEPS)
    file(MAKE_DIRECTORY "${GENERATED_OUTPUT_PATH}")

    # Temporary input lists for the code generator
    set(SRC_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/sourcefiles")
    set(INC_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/includedirectories")

    write_all_sources("${SRC_LIST_FILE}" "${PUBLIC_SOURCES}")
    write_all_includes_for_target(
        ${TARGET}
        "${INC_LIST_FILE}"
        "${PUBLIC_DEPS}"
        "${PRIVATE_DEPS}"
    )
    
    add_custom_command(
        OUTPUT "${GENERATED_HEADER_PATH}" "${GENERATED_CPP_PATH}"
        COMMAND ${ONYX_CODEGEN} "--module"
                "${TARGET}"
                "${TARGET_NAMESPACE}"
                "${CMAKE_CURRENT_SOURCE_DIR}"
                "${GENERATED_PUBLIC_PATH}"
                "${GENERATED_PRIVATE_PATH}"
                "${SRC_LIST_FILE}"
                "${INC_LIST_FILE}"
        DEPENDS
            ${PUBLIC_SOURCES}           # If source changes, regenerate
            "${INC_LIST_FILE}"          # If include dirs change, regenerate
            "${ONYX_CODEGEN}"  # generator changed
        COMMENT "Running Onyx code generation for ${TARGET}"
        VERBATIM
    )
    
    set_property(GLOBAL APPEND PROPERTY onyx_generated_module_headers "${GENERATED_HEADER_PATH}")
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

    # Remove empties
    list(FILTER all_raw EXCLUDE REGEX "^$")

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

        #get_target_property(priv "${current}" LINK_LIBRARIES)
        #if(priv)
        #    foreach(lib IN LISTS priv)
        #        if(TARGET "${lib}" AND NOT lib IN_LIST visited)
        #            list(APPEND queue "${lib}")
        #        endif()
        #    endforeach()
        #endif()
    endwhile()

    # Remove original target from result
    list(REMOVE_ITEM visited "${TARGET}")

    set(${RESULT} "${visited}" PARENT_SCOPE)
endfunction()