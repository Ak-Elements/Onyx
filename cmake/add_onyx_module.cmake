function(onyx_add_target TARGET_NAME)
     set(options NO_CODEGEN NO_EDITOR_TARGET)  # No boolean options
     set(oneValueArgs NAMESPACE TARGET_TYPE FOLDER ALIAS)
     set(multiValueArgs PUBLIC_DEPENDENCIES PRIVATE_DEPENDENCIES PUBLIC_DEFINES PRIVATE_DEFINES)
     
     #set(arg_ENABLE_CODEGEN true)
     cmake_parse_arguments(
         arg
         "${options}"
         "${oneValueArgs}"
         "${multiValueArgs}"
         ${ARGN}
     )
     
    if (NOT arg_NAMESPACE)
        set(arg_NAMESPACE "${TARGET_NAME}")
    endif()

    onyx_create_target(
        TARGET ${TARGET_NAME}
        NAMESPACE ${arg_NAMESPACE}
        TYPE ${arg_TARGET_TYPE}
        FOLDER ${arg_FOLDER}
        ALIAS ${arg_ALIAS}
        BASE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        BASE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}
        PUBLIC_DEFINES ${arg_PUBLIC_DEFINES}
        PRIVATE_DEFINES ${arg_PRIVATE_DEFINES}
        PUBLIC_DEPENDENCIES ${arg_PUBLIC_DEPENDENCIES}
        PRIVATE_DEPENDENCIES ${arg_PRIVATE_DEPENDENCIES}
    )

    get_target_property(runtime_target_ocd_files ${TARGET_NAME} ONYX_OCD_FILES)
    if (runtime_target_ocd_files STREQUAL "runtime_target_ocd_files-NOTFOUND")
        set(runtime_target_ocd_files "")
    endif()

    if (NOT arg_NO_EDITOR_TARGET AND (runtime_target_ocd_files OR EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/editor"))
        onyx_create_target(
            TARGET ${TARGET_NAME}-editor
            NAMESPACE ${arg_NAMESPACE}
            TYPE ${arg_TARGET_TYPE}
            FOLDER editor
            BASE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/editor
            BASE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/editor
            PRIVATE_DEPENDENCIES ${TARGET_NAME}
        )
        # adjust namespace after the create target to not change namespace folder path
        set_target_properties(${TARGET_NAME}-editor PROPERTIES ONYX_NAMESPACE "${arg_NAMESPACE}::Editor")

    endif()

    if (NOT arg_NO_CODEGEN)
        onyx_add_codegen_for_target(
            TARGET ${TARGET_NAME}
            BASE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}
            BASE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
            PUBLIC_DIR_SUFFIX "public/${ns_path}"
            PRIVATE_DIR_SUFFIX "private/${ns_path}"
            GENERATED_DIR_SUFFIX "generated"
            IS_EDITOR FALSE
        )

        if (TARGET ${TARGET_NAME}-editor)
            onyx_add_codegen_for_target(
                TARGET ${TARGET_NAME}-editor
                RUNTIME_TARGET ${TARGET_NAME}
                BASE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/editor
                BASE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/editor
                PUBLIC_DIR_SUFFIX "public/${ns_path}"
                PRIVATE_DIR_SUFFIX "private/${ns_path}"
                GENERATED_DIR_SUFFIX "generated"
                IS_EDITOR TRUE
                
            )
        endif()
    endif()

    onyx_install_targets(${TARGET_NAME})
endfunction()

function(onyx_create_target)
    set(oneValueArgs
        TARGET
        NAMESPACE
        TYPE
        FOLDER
        ALIAS
        BASE_SOURCE_DIR
        BASE_BINARY_DIR
    )

    set(multiValueArgs
        PUBLIC_SOURCES
        PRIVATE_SOURCES
        PUBLIC_DEPENDENCIES
        PRIVATE_DEPENDENCIES
        PUBLIC_DEFINES
        PRIVATE_DEFINES
    )

    cmake_parse_arguments(arg "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    #### Target type ####
    if (arg_TYPE STREQUAL "EXECUTABLE")
        add_executable(${arg_TARGET})
    else()
        add_library(${arg_TARGET} STATIC)
    endif()

    if (arg_ALIAS)
        add_library(${arg_ALIAS} ALIAS ${arg_TARGET})
    endif()

    #### Namespace path ####
    string(REPLACE "::" "/" ns_path "${arg_NAMESPACE}")
    string(TOLOWER "${ns_path}" ns_path)

    #### Source dirs ####
    set(public_dir "${arg_BASE_SOURCE_DIR}/public/${ns_path}")
    set(private_dir "${arg_BASE_SOURCE_DIR}/private/${ns_path}")

    if (NOT EXISTS "${public_dir}")
        set(public_dir "${arg_BASE_SOURCE_DIR}")
    endif()

    if (NOT EXISTS "${private_dir}")
        set(private_dir "${arg_BASE_SOURCE_DIR}")
    endif()

    #### Sources ####
    if (EXISTS "${arg_BASE_SOURCE_DIR}/source_definitions.cmake")
        include("${arg_BASE_SOURCE_DIR}/source_definitions.cmake")
        list(APPEND arg_PUBLIC_SOURCES  ${onyx_TARGET_PUBLIC_SOURCES})
        list(APPEND arg_PRIVATE_SOURCES ${onyx_TARGET_PRIVATE_SOURCES})
        
        if (onyx_TARGET_PCH)
            if(IS_ABSOLUTE "${onyx_TARGET_PCH}")
                set(target_precompiled_header "${onyx_TARGET_PCH}")
            else()
                cmake_path(APPEND public_dir ${onyx_TARGET_PCH} OUTPUT_VARIABLE target_precompiled_header)
            endif()
        endif()
    endif()

    _onyx_normalize_source_list(arg_PUBLIC_SOURCES  "${public_dir}")
    _onyx_normalize_source_list(arg_PRIVATE_SOURCES "${private_dir}")

    if (arg_PUBLIC_SOURCES)
        target_sources(${arg_TARGET} PUBLIC
            FILE_SET HEADERS
            BASE_DIRS "${arg_BASE_SOURCE_DIR}/public"
            FILES ${arg_PUBLIC_SOURCES}
        )
    endif()

    if (arg_PRIVATE_SOURCES)
        target_sources(${arg_TARGET} PRIVATE ${arg_PRIVATE_SOURCES})
    endif()

    #### Precompiled Header ####
    if (target_precompiled_header)
        target_precompile_headers(${arg_TARGET} PUBLIC ${target_precompiled_header})
    endif()

    #### OCD Files ####
    set(target_odc_files ${arg_PUBLIC_SOURCES})
    list(FILTER target_odc_files INCLUDE REGEX ".*\.${ONYX_COMPONENT_DEFINITION_FILE_EXTENSION}")

    #### Dependencies ####
    if (EXISTS "${arg_BASE_SOURCE_DIR}/dependencies.cmake")
        include("${arg_BASE_SOURCE_DIR}/dependencies.cmake")
        list(APPEND arg_PUBLIC_DEPENDENCIES  ${onyx_TARGET_PUBLIC_DEPENDENCIES})
        list(APPEND arg_PRIVATE_DEPENDENCIES ${onyx_TARGET_PRIVATE_DEPENDENCIES})
    endif()

    #### Properties ####
    set_target_properties(${arg_TARGET} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        FOLDER "${arg_FOLDER}"
        EXPORT_NAME "${arg_NAMESPACE}"
    )

    #### Defines ####
    target_compile_definitions(${arg_TARGET} PUBLIC ${ONYX_PUBLIC_DEFINES})
    target_compile_definitions(${arg_TARGET} PRIVATE ${ONYX_PRIVATE_DEFINES})

    if (arg_PUBLIC_DEFINES)
        target_compile_definitions(${arg_TARGET} PUBLIC ${arg_PUBLIC_DEFINES})
    endif()

    if (arg_PRIVATE_DEFINES)
        target_compile_definitions(${arg_TARGET} PRIVATE ${arg_PRIVATE_DEFINES})
    endif()

    #### Compile & Link Options ####
    target_compile_options(${arg_TARGET} PRIVATE ${ONYX_COMPILE_OPTIONS})
    target_link_options(${arg_TARGET} PRIVATE ${ONYX_LINK_OPTIONS})

    #### Dependencies ####
    if (arg_PUBLIC_DEPENDENCIES)
        target_link_libraries(${arg_TARGET} PUBLIC ${arg_PUBLIC_DEPENDENCIES})
    endif()

    if (arg_PRIVATE_DEPENDENCIES)
        target_link_libraries(${arg_TARGET} PRIVATE ${arg_PRIVATE_DEPENDENCIES})
    endif()

    #### Store metadata for later steps ####
    set_target_properties(${arg_TARGET} PROPERTIES
        ONYX_NAMESPACE       "${arg_NAMESPACE}"
        ONYX_NS_PATH         "${ns_path}"
        ONYX_PUBLIC_SOURCES  "${arg_PUBLIC_SOURCES}"
        ONYX_PRIVATE_SOURCES "${arg_PRIVATE_SOURCES}"
        ONYX_PUBLIC_DEPENDENCIES "${arg_PUBLIC_DEPENDENCIES}"
        ONYX_PRIVATE_DEPENDENCIES "${arg_PRIVATE_DEPENDENCIES}"
        ONYX_OCD_FILES      "${target_odc_files}"
    )
endfunction()

function(_onyx_add_target)

    set(oneValueArgs
        name
        namespace
        type
        folder
        alias
        base_source_dir
        base_binary_dir
        out_public_sources
        out_public_dependencies
        out_private_dependencies
    )

    set(multiValueArgs
        public_sources
        private_sources
        public_dependencies
        private_dependencies
        public_defines
        private_defines
        
    )

    set(arg_ENABLE_CODEGEN true)
    cmake_parse_arguments(
        "target"
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    #### Target ####
    if (target_type AND target_type STREQUAL "EXECUTABLE")
        add_executable(${target_name})
        set(is_executable True)
        if (target_alias)
            add_executable("${target_alias}" ALIAS ${target_name})
        endif()
    else()
        add_library(${target_name} STATIC)
        
        set(is_executable False)
        if (target_alias)
            add_library("${target_alias}" ALIAS ${target_name})
        endif()
    endif()

    set(target_public_sources_dir_suffix "")
    cmake_path(APPEND target_base_source_dir "public/${target_ns_path}" OUTPUT_VARIABLE target_public_sources_dir)
    if (EXISTS "${target_public_sources_dir}")
        set(target_public_include_dir "${target_base_source_dir}/public")
        set(target_public_sources_dir_suffix "public/${target_ns_path}")
    else()
        set(target_public_sources_dir ${target_base_source_dir})
        set(target_public_include_dir ${target_base_source_dir})
        set(target_public_sources_dir_suffix "")
    endif()
    
    cmake_path(APPEND target_binary_dir "generated/public/${target_ns_path}" OUTPUT_VARIABLE target_public_binary_dir)

    set(target_private_sources_dir_suffix "")
    cmake_path(APPEND target_base_source_dir "private/${target_ns_path}" OUTPUT_VARIABLE target_private_sources_dir)
    if (EXISTS "${target_private_sources_dir}")
        set(target_private_sources_dir_suffix "private/${target_ns_path}")
    else()
        set(target_target_private_sources_dir ${target_base_source_dir})
    endif()
    
    cmake_path(APPEND target_binary_dir "generated/private/${target_ns_path}" OUTPUT_VARIABLE target_private_binary_dir)

    if (EXISTS ${target_base_source_dir}/source_definitions.cmake)
        include(${target_base_source_dir}/source_definitions.cmake)
        list(APPEND target_public_sources ${onyx_TARGET_PUBLIC_SOURCES})
        list(APPEND target_private_sources ${onyx_TARGET_PRIVATE_SOURCES})

        if (onyx_TARGET_PCH)
            if(IS_ABSOLUTE "${onyx_TARGET_PCH}")
                set(target_precompiled_header "${onyx_TARGET_PCH}")
            else()
                cmake_path(APPEND target_public_sources_dir ${onyx_TARGET_PCH} OUTPUT_VARIABLE target_precompiled_header)
            endif()
        endif()
    endif()

    if (EXISTS ${target_base_source_dir}/dependencies.cmake)
        message(STATUS "[${target_name}] Getting dependencies.")
        include(${target_base_source_dir}/dependencies.cmake)
        message(STATUS "[${target_name}] Finished getting dependencies.")

        list(APPEND target_public_dependencies ${onyx_TARGET_PUBLIC_DEPENDENCIES})
        list(APPEND target_private_dependencies ${onyx_TARGET_PRIVATE_DEPENDENCIES})
    endif()

    _onyx_normalize_source_list(target_public_sources "${target_public_sources_dir}")
    _onyx_normalize_source_list(target_private_sources "${target_private_sources_dir}")

    list(REMOVE_DUPLICATES target_public_dependencies)
    list(REMOVE_DUPLICATES target_private_dependencies)
    
    # Store module name in a global property
    set_property(GLOBAL APPEND PROPERTY onyx_targets "${arg_NAMESPACE}")

    if(target_public_sources)
        target_sources(${target_name} PUBLIC
            FILE_SET HEADERS
            BASE_DIRS "${target_public_include_dir}"
            FILES ${target_public_sources}
        )

        source_group(TREE ${target_public_sources_dir} FILES ${target_public_sources})
    endif()

    if(target_private_sources)
        target_sources(${target_name} PRIVATE ${target_private_sources})
        source_group(TREE ${target_private_sources_dir} FILES ${target_private_sources})
    endif()
    
    #### Target Properties ####
    set_target_properties(${target_name}
        PROPERTIES
            CXX_STANDARD 23
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES
            SOVERSION ${MAJOR_VERSION}
            VERSION ${PROJECT_VERSION}
    )

    if (target_namespace)
        set_target_properties(${target_name} PROPERTIES EXPORT_NAME "${target_namespace}")
    endif()

    if (target_folder)
        set_target_properties(${target_name} PROPERTIES FOLDER "${target_folder}")
    endif()
    
    #### Precompiled Header ####
    if (target_precompiled_header)
        target_precompile_headers(${target_name} PUBLIC ${target_precompiled_header})
    endif()

    #### Compiler Defines ####
    target_compile_definitions(${target_name} PUBLIC ${ONYX_PUBLIC_DEFINES})
    target_compile_definitions(${target_name} PRIVATE ${ONYX_PRIVATE_DEFINES})

    if (target_public_defines)
        target_compile_definitions(${target_name} PUBLIC ${target_public_defines})
    endif()

     if (target_private_defines)
        target_compile_definitions(${target_name} PRIVATE ${target_private_defines})
    endif()

    #### Compiler & Link Options ####
    target_compile_options(${target_name} PRIVATE ${ONYX_COMPILE_OPTIONS})
    target_link_options(${target_name} PRIVATE ${ONYX_LINK_OPTIONS})

    target_include_directories(${target_name} PRIVATE
        $<BUILD_INTERFACE:${target_base_source_dir}/private>
        $<INSTALL_INTERFACE:source>
    )

    #### Dependencies ####
    if(target_public_dependencies)
        target_link_libraries(${target_name} PUBLIC ${target_public_dependencies})
    endif()

    if(target_private_dependencies)
        target_link_libraries(${target_name} PRIVATE ${target_private_dependencies})
    endif() 

    set(${target_out_public_sources} ${target_public_sources} PARENT_SCOPE)
    set(${target_out_public_dependencies} ${target_public_dependencies} PARENT_SCOPE)
    set(${target_out_private_dependencies} ${target_private_dependencies} PARENT_SCOPE)
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

function(onyx_install_targets TARGET)
    if (NOT ONYX_ENABLE_INSTALL)
        return()
    endif()

    set(installtargets ${TARGET})
    if (TARGET ${TARGET}-editor)
        list(APPEND installtargets ${TARGET}-editor)
    endif()
    #### Install ####
    include(GNUInstallDirs)
    install(TARGETS ${installtargets}
        EXPORT ${TARGET}-targets
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
        ${TARGET}-targets
        DESTINATION ${ONYX_INSTALL_CMAKEDIR}
        NAMESPACE "${namespace}::"
        FILE ${arg_TARGET_NAME}-targets.cmake
        COMPONENT ${PROJECT_NAME}_Development
    )
endfunction()