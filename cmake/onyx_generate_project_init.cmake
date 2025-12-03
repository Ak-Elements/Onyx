function(onyx_generate_project_init ONYX_PROJECT_NAME)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    get_property(onyx_generated_module_headers GLOBAL PROPERTY onyx_generated_module_headers)
    set(GENERATED_CPP_PATH "${CMAKE_CURRENT_BINARY_DIR}/engine_init.gen.cpp")
    # Temporary input lists for the code generator
    set(ONYX_CODEGEN "${onyx_SOURCE_DIR}/tools/codegeneration/onyx-codegen")
    if(WIN32)
        set(ONYX_CODEGEN "${ONYX_CODEGEN}.exe")
    endif()

    write_all_modules("${CMAKE_CURRENT_BINARY_DIR}/generatedmoduleheaders" "${onyx_generated_module_headers}")

    add_custom_command(
        OUTPUT "${GENERATED_CPP_PATH}"
        COMMAND ${ONYX_CODEGEN} "--project"
                "${GENERATED_CPP_PATH}"
                "${onyx_BINARY_DIR}"
                "${CMAKE_CURRENT_BINARY_DIR}"
                "${CMAKE_CURRENT_BINARY_DIR}/generatedmoduleheaders"
        DEPENDS
            ${onyx_generated_modules} 
            "${ONYX_CODEGEN}"  # generator changed
        COMMENT "Running Onyx code generation for ${TARGET}"
        VERBATIM
    )

   # add_custom_target("${ONYX_PROJECT_NAME}_code_gen"
   #     DEPENDS "${GENERATED_CPP_PATH}"
   # )

    set_target_properties("${ONYX_PROJECT_NAME}_code_gen"
        PROPERTIES
            FOLDER "${ONYX_CODEGENERATION_TARGETS_FOLDER}"
            SOVERSION ${MAJOR_VERSION}
            VERSION ${PROJECT_VERSION}
    )

    # Make module depend on the generated code
    add_dependencies(${ONYX_PROJECT_NAME} "${ONYX_PROJECT_NAME}_code_gen")
endfunction()

function(write_all_modules FILE MODULES)
    # Convert to newline-separated output
    string(REPLACE ";" "\n" text "${MODULES}")
    file(WRITE "${FILE}" "${text}")
endfunction()