function(onyx_add_target arg_TARGET_NAME)
    set(options NO_CODEGEN NO_EDITOR_TARGET)  # No boolean options
    set(oneValueArgs NAMESPACE TARGET_TYPE FOLDER ALIAS)
    set(multiValueArgs PUBLIC_DEPENDENCIES PRIVATE_DEPENDENCIES PUBLIC_DEFINES PRIVATE_DEFINES)

    set(arg_ENABLE_CODEGEN true)
    cmake_parse_arguments(
        arg
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    
    # force this to off for now to avoid wrong build dependencies
    set(arg_NO_EDITOR_TARGET true)

    if (arg_NO_CODEGEN)
        set(arg_ENABLE_CODEGEN false)
    endif()
    
    if (NOT arg_NAMESPACE)
        set(arg_NAMESPACE "${arg_TARGET_NAME}")
    endif()

    string(REPLACE "::" "/" target_ns_path "${arg_NAMESPACE}")
    string(TOLOWER "${target_ns_path}" target_ns_path)

    _onyx_add_target(
        name ${arg_TARGET_NAME}
        namespace ${arg_TARGET_NAMESPACE}
        type ${arg_TARGET_TYPE}
        alias ${arg_ALIAS}
        folder ${arg_FOLDER}
        base_source_dir ${CMAKE_CURRENT_SOURCE_DIR}
        base_binary_dir ${CMAKE_CURRENT_BINARY_DIR}
        public_sources ${onyx_TARGET_PUBLIC_SOURCES}
        private_sources ${onyx_TARGET_PRIVATE_SOURCES}
        public_dependencies ${onyx_TARGET_PUBLIC_DEPENDENCIES}
        private_dependencies ${onyx_TARGET_PRIVATE_DEPENDENCIES}
        public_defines ${arg_PUBLIC_DEFINES}
        private_defines ${arg_PRIVATE_DEFINES}
        out_public_sources target_public_sources
        out_public_dependencies target_public_dependencies
        out_private_dependencies target_private_dependencies
    )

    set(has_editor_target False)
    if (NOT arg_NO_EDITOR_TARGET)
        set(has_editor_target True)
        
        if (arg_ALIAS)
            set(editor_alias ${arg_ALIAS}-editor)
        endif()
        _onyx_add_target(
            name ${arg_TARGET_NAME}-editor
            type ${arg_TARGET_TYPE}
            namespace ${arg_TARGET_NAMESPACE}
            alias ${editor_alias}
            folder editor
            base_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/editor
            base_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/editor
            public_defines ${arg_PUBLIC_DEFINES}
            private_defines ${arg_PRIVATE_DEFINES}
        )

        target_link_libraries(${arg_TARGET_NAME}-editor PUBLIC ${arg_TARGET_NAME} onyx-editor)
    endif()

    _onyx_target_codegeneration()
    _onyx_target_install()

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

    #### Target ####
    if (target_type AND target_type STREQUAL "EXECUTABLE")
        add_executable(${target_name})
        set(is_executable True)
        if (target_alias)
            add_executable("${target_alias}" ALIAS ${target_name})
        endif()
    else()
        #if (TARGET ${target_name} AND NOT ${target_name} STREQUAL onyx-editor)
        #    message(FATAL_ERROR "Target already defined")
        #    return()
        #endif()

        #if (NOT ${target_name} STREQUAL onyx-editor)
        add_library(${target_name} STATIC)
        #endif()
        
        set(is_executable False)
        if (target_alias)
            add_library("${target_alias}" ALIAS ${target_name})
        endif()

    endif()
    
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

function(_onyx_target_install)
    if (NOT ONYX_ENABLE_INSTALL)
        return()
    endif()

    set(installtargets ${arg_TARGET_NAME})
    if (has_editor_target)
        list(APPEND installtargets ${arg_TARGET_NAME}-editor)
    endif()
    #### Install ####
    include(GNUInstallDirs)
    install(TARGETS ${installtargets}
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
endfunction()

function(_onyx_target_codegeneration)
    if (NOT arg_ENABLE_CODEGEN)
        return()
    endif()
    # add the generated sources folder to the include directories for the generator to be able generate correct includes
    target_sources(${arg_TARGET_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/generated/public"
    )

    set(target_public_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/public/${target_ns_path}")
    set(target_private_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/private/${target_ns_path}")
    set(target_editor_public_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/editor/generated/private/${target_ns_path}")
    set(target_editor_private_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/editor/generated/private/${target_ns_path}")

    # make generated source folders
    file(MAKE_DIRECTORY ${target_public_binary_dir})
    file(MAKE_DIRECTORY ${target_private_binary_dir})
    file(MAKE_DIRECTORY ${target_editor_public_binary_dir})
    file(MAKE_DIRECTORY ${target_editor_private_binary_dir})

    #### Module Code Generation Target ####
    onyx_add_code_gen_target(
        TARGET ${arg_TARGET_NAME}
        EDITOR_TARGET ${arg_TARGET_NAME}-editor
        TARGET_TYPE ${arg_TARGET_TYPE}
        NAMESPACE ${arg_NAMESPACE}
        PUBLIC_SOURCES ${target_public_sources}
        PUBLIC_DEPENDENCIES ${target_public_dependencies}
        PRIVATE_DEPENDENCIES ${target_private_dependencies}
        PUBLIC_DIR_SUFFIX  "public/${target_ns_path}"
        PRIVATE_DIR_SUFFIX "private/${target_ns_path}"
        GENERATED_DIR_SUFFIX  "generated"
        OUT_PUBLIC_SOURCES generated_public_sources
        OUT_PRIVATE_SOURCES generated_private_sources
        OUT_PUBLIC_EDITOR_SOURCES generated_public_editor_sources
        OUT_PRIVATE_EDITOR_SOURCES generated_private_editor_sources
    )

    target_sources(${arg_TARGET_NAME} PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/generated/public"
        FILES ${generated_public_sources}
    )
    target_sources(${arg_TARGET_NAME} PRIVATE ${generated_private_sources})

    source_group(TREE ${target_public_binary_dir} FILES ${generated_public_sources})
    source_group(TREE ${target_private_binary_dir} FILES ${generated_private_sources})

    if (has_editor_target)
        ## EDITOR
        target_sources(${arg_TARGET_NAME}-editor PUBLIC
            FILE_SET HEADERS
            BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/editor/generated/public"
            FILES ${generated_public_editor_sources}
        )
        target_sources(${arg_TARGET_NAME}-editor PRIVATE ${generated_private_editor_sources})
        
        source_group(TREE ${target_editor_public_binary_dir} FILES ${generated_public_editor_sources})
        source_group(TREE ${target_editor_private_binary_dir} FILES ${generated_private_editor_sources})
    endif()
endfunction()