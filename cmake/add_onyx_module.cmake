function(onyx_add_target arg_TARGET_NAME)
    set(options "")  # No boolean options
    set(oneValueArgs NAMESPACE PRECOMPILED_HEADER TARGET_TYPE FOLDER ALIAS)
    set(multiValueArgs PUBLIC_SOURCES PRIVATE_SOURCES PUBLIC_DEPENDENCIES PRIVATE_DEPENDENCIES PUBLIC_DEFINES PRIVATE_DEFINES)

    cmake_parse_arguments(
        arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    
    if (NOT arg_NAMESPACE)
        set(arg_NAMESPACE "${arg_TARGET_NAME}")
    endif()

    string(REPLACE "::" "/" target_ns_path "${arg_NAMESPACE}")
    string(TOLOWER "${target_ns_path}" target_ns_path)
    
    if (NOT arg_PUBLIC_BASE_DIR)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/public")
            set(arg_PUBLIC_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/public")
        else()
            set(arg_PUBLIC_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif()
    endif()

    if (NOT arg_PUBLIC_SOURCES_DIR)

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/public/${target_ns_path}")
            set(arg_PUBLIC_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/public/${target_ns_path}")
        else()
            set(arg_PUBLIC_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif()
        
        set(arg_PUBLIC_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/public/${module_ns_path}")
    endif()

    if (NOT arg_PRIVATE_SOURCES_DIR)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/private/${target_ns_path}")
            set(arg_PRIVATE_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/private/${target_ns_path}")
        else()
            set(arg_PRIVATE_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        endif()
        
        set(arg_PRIVATE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/private/${module_ns_path}")
    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/source_definitions.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/source_definitions.cmake)
        list(APPEND arg_PUBLIC_SOURCES ${onyx_TARGET_PUBLIC_SOURCES})
        list(APPEND arg_PRIVATE_SOURCES ${onyx_TARGET_PRIVATE_SOURCES})

        if (arg_PRECOMPILED_HEADER)
            if (ONYX_TARGET_PCH)
                message(WARNING "Module ${arg_TARGET_NAME} has a precompiled header passed as argument and defined in the sources list. Ignoring the PCH from the sources ")
            endif()
        else()
            if (onyx_TARGET_PCH)
                if(IS_ABSOLUTE "${onyx_TARGET_PCH}")
                    set(arg_PRECOMPILED_HEADER "${onyx_TARGET_PCH}")
                else()
                    cmake_path(APPEND arg_PUBLIC_SOURCES_DIR ${onyx_TARGET_PCH} OUTPUT_VARIABLE arg_PRECOMPILED_HEADER)
                endif()
            endif()
        endif()

    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake)
        message(STATUS "[${arg_TARGET_NAME}] Getting dependencies.")
        include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake)
        message(STATUS "[${arg_TARGET_NAME}] Finished getting dependencies.")

        list(APPEND arg_PUBLIC_DEPENDENCIES ${onyx_TARGET_PUBLIC_DEPENDENCIES})
        list(APPEND arg_PRIVATE_DEPENDENCIES ${onyx_TARGET_PRIVATE_DEPENDENCIES})
    endif()

    _onyx_normalize_source_list(arg_PUBLIC_SOURCES "${arg_PUBLIC_SOURCES_DIR}")
    _onyx_normalize_source_list(arg_PRIVATE_SOURCES "${arg_PRIVATE_SOURCES_DIR}")

    list(REMOVE_DUPLICATES arg_PUBLIC_DEPENDENCIES)
    list(REMOVE_DUPLICATES arg_PRIVATE_DEPENDENCIES)

    #### Target ####
    if (arg_TARGET_TYPE AND arg_TARGET_TYPE STREQUAL "EXECUTABLE")
        add_executable(${arg_TARGET_NAME})

        if (arg_ALIAS)
            add_executable("${arg_ALIAS}" ALIAS ${arg_TARGET_NAME})
        endif()
    else()
        add_library(${arg_TARGET_NAME} STATIC)

        if (arg_ALIAS)
            add_library("${arg_ALIAS}" ALIAS ${arg_TARGET_NAME})
        endif()
    endif()
    
    # Store module name in a global property
    set_property(GLOBAL APPEND PROPERTY onyx_targets "${arg_NAMESPACE}")

    set(GENERATED_HEADER_PATH "${arg_PUBLIC_BINARY_DIR}/${arg_TARGET_NAME}.h")
    set(GENERATED_CPP_PATH "${arg_PRIVATE_BINARY_DIR}/${arg_TARGET_NAME}.cpp")

    if(arg_PUBLIC_SOURCES)
        target_sources(${arg_TARGET_NAME} PUBLIC
            FILE_SET HEADERS
            BASE_DIRS "${arg_PUBLIC_BASE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/generated/public"
            FILES ${arg_PUBLIC_SOURCES} ${GENERATED_HEADER_PATH}
        )
        
        source_group(TREE ${arg_PUBLIC_SOURCES_DIR} FILES ${arg_PUBLIC_SOURCES})
        source_group(TREE ${arg_PUBLIC_BINARY_DIR} FILES ${GENERATED_HEADER_PATH})
    endif()

    if(arg_PRIVATE_SOURCES)
        target_sources(${arg_TARGET_NAME} PRIVATE ${arg_PRIVATE_SOURCES} ${GENERATED_CPP_PATH})

        source_group(TREE ${arg_PRIVATE_SOURCES_DIR} FILES ${arg_PRIVATE_SOURCES})
        source_group(TREE "${arg_PRIVATE_BINARY_DIR}" FILES ${GENERATED_CPP_PATH})
    endif()
    
    #### Target Properties ####
    set_target_properties(${arg_TARGET_NAME}
        PROPERTIES
            CXX_STANDARD 23
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES
            SOVERSION ${MAJOR_VERSION}
            VERSION ${PROJECT_VERSION}
    )

    if (arg_NAMESPACE)
        set_target_properties(${arg_TARGET_NAME} PROPERTIES EXPORT_NAME "${arg_NAMESPACE}")
    endif()

    if (arg_FOLDER)
        set_target_properties(${arg_TARGET_NAME} PROPERTIES FOLDER "${arg_FOLDER}")
    endif()
    
    #### Precompiled Header ####
    if (arg_PRECOMPILED_HEADER)
        target_precompile_headers(${arg_TARGET_NAME} PUBLIC ${arg_PRECOMPILED_HEADER})
    endif()

    #### Compiler Defines ####
    target_compile_definitions(${arg_TARGET_NAME} PUBLIC ${ONYX_PUBLIC_DEFINES})
    target_compile_definitions(${arg_TARGET_NAME} PRIVATE ${ONYX_PRIVATE_DEFINES})

    if (DEFINED arg_PUBLIC_DEFINES)
        target_compile_definitions(${arg_TARGET_NAME} PUBLIC ${arg_PUBLIC_DEFINES})
    endif()

     if (DEFINED arg_PRIVATE_DEFINES)
        target_compile_definitions(${arg_TARGET_NAME} PRIVATE ${arg_PRIVATE_DEFINES})
    endif()

    #### Compiler & Link Options ####
    target_compile_options(${arg_TARGET_NAME} PRIVATE ${ONYX_COMPILE_OPTIONS})
    target_link_options(${arg_TARGET_NAME} PRIVATE ${ONYX_LINK_OPTIONS})

    target_include_directories(${arg_TARGET_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
        $<INSTALL_INTERFACE:source>
    )

    #### Dependencies ####
    if(arg_PUBLIC_DEPENDENCIES)
        target_link_libraries(${arg_TARGET_NAME} PUBLIC ${arg_PUBLIC_DEPENDENCIES})
    endif()

    if(arg_PRIVATE_DEPENDENCIES)
        target_link_libraries(${arg_TARGET_NAME} PRIVATE ${arg_PRIVATE_DEPENDENCIES})
    endif()
    
    #### Module Code Generation Target ####
    onyx_add_code_gen_target(${arg_TARGET_NAME}
        ${arg_NAMESPACE}
        ${arg_PUBLIC_BINARY_DIR}
        ${arg_PRIVATE_BINARY_DIR}
        ${GENERATED_HEADER_PATH}
        ${GENERATED_CPP_PATH}
        "${arg_PUBLIC_SOURCES}"
        "${arg_PUBLIC_DEPENDENCIES}"
        "${arg_PRIVATE_DEPENDENCIES}"
    )
 
if (ONYX_ENABLE_INSTALL)
    #### Install ####
    include(GNUInstallDirs)
    install(TARGETS ${arg_TARGET_NAME}
        EXPORT ${arg_TARGET_NAME}-targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET HEADERS
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT ${PROJECT_NAME}_RunTime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_RunTime
            NAMELINK_COMPONENT ${PROJECT_NAME}_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_Development
    )

    string(FIND ${arg_NAMESPACE} "::" index)
    if(${index} EQUAL "-1")
        set(namespace "${arg_NAMESPACE}")
    else()
        string(SUBSTRING ${arg_NAMESPACE} 0 ${index} namespace)
        
    endif()
    
    install(EXPORT 
        ${arg_TARGET_NAME}-targets
        DESTINATION ${ONYX_INSTALL_CMAKEDIR}
        NAMESPACE "${namespace}::"
        FILE ${arg_TARGET_NAME}-targets.cmake
        COMPONENT ${PROJECT_NAME}_Development
    )
endif()

endfunction()

function(_onyx_normalize_source_list list_var base_dir)
    # Expand into local variable
    set(_list "${${list_var}}")

    # Remove duplicates
    list(REMOVE_DUPLICATES _list)

    set(_out_list "")
    foreach(entry IN LISTS _list)
    if(NOT IS_ABSOLUTE "${entry}")
        cmake_path(APPEND base_dir ${entry} OUTPUT_VARIABLE path)
        list(APPEND _out_list "${path}")
    else()
        list(APPEND _out_list "${entry}")
    endif()
endforeach()

    set(${list_var} "${_out_list}" PARENT_SCOPE)
endfunction()