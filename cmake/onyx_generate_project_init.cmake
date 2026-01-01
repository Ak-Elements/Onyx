function(onyx_generate_project_init ONYX_PROJECT_NAME)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    get_property(onyx_generated_module_headers GLOBAL PROPERTY onyx_generated_module_headers)
    set(GENERATED_CPP_PATH "${CMAKE_CURRENT_BINARY_DIR}/init.gen.cpp")
    # Temporary input lists for the code generator

    write_all_modules("${CMAKE_CURRENT_BINARY_DIR}/generatedmoduleheaders" "${onyx_generated_module_headers}")
    
    add_custom_command(
        OUTPUT "${GENERATED_CPP_PATH}"
        COMMAND ${ONYX_CODEGEN} ${CMAKE_CURRENT_BINARY_DIR}/config-codegen.toml
        DEPENDS
            ${onyx_generated_module_headers} 
            "${ONYX_CODEGEN}"  # generator changed
        COMMENT "Running Onyx project code generation for ${ONYX_PROJECT_NAME}"
        VERBATIM
    )
    
    # Add the generated CPP file to the executable's sources
    target_sources(${ONYX_PROJECT_NAME} PRIVATE ${GENERATED_CPP_PATH})

endfunction()

function(write_all_modules FILE MODULES)
    # Convert to newline-separated output
    string(REPLACE ";" "\n" text "${MODULES}")
    file(WRITE "${FILE}" "${text}")
endfunction()