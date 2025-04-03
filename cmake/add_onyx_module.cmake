function(add_onyx_module ONYX_TARGET_NAME)
    
    # Parse arguments using cmake_parse_arguments
    cmake_parse_arguments(
        TARGET                  # The prefix for the parsed arguments
        ""                      # No options
        "PRECOMPILED_HEADER"    # single-value arguments
        "PUBLIC_SOURCES;PRIVATE_SOURCES;PUBLIC_DEPENDENCIES;PRIVATE_DEPENDENCIES;PUBLIC_DEFINES;PRIVATE_DEFINES"  # Multi-value arguments (source and dependency lists)
        ${ARGN}                 # Remaining arguments
    )

    #### Target ####
    
    string(SUBSTRING ${ONYX_TARGET_NAME} 5 -1 ONYX_TARGET_EXPORT_NAME)
    add_library(${ONYX_TARGET_NAME} STATIC)
    add_library(onyx::${ONYX_TARGET_EXPORT_NAME} ALIAS ${ONYX_TARGET_NAME})

    # Store module name in a global property
    
    set_property(GLOBAL APPEND PROPERTY onyx_modules "${ONYX_TARGET_EXPORT_NAME}")


    if(TARGET_PUBLIC_SOURCES)
        target_sources(${ONYX_TARGET_NAME} PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/public
            FILES ${TARGET_PUBLIC_SOURCES}
        )
        source_group(TREE ${TARGET_PUBLIC_PATH} FILES ${TARGET_PUBLIC_SOURCES})
    endif()

    if(TARGET_PRIVATE_SOURCES)
        target_sources(${ONYX_TARGET_NAME} PRIVATE ${TARGET_PRIVATE_SOURCES})
        source_group(TREE ${TARGET_PRIVATE_PATH} FILES ${TARGET_PRIVATE_SOURCES})
    endif()
    
    #### Target Properties ####
    set_target_properties(${ONYX_TARGET_NAME}
        PROPERTIES
            EXPORT_NAME ${ONYX_TARGET_EXPORT_NAME}
            FOLDER "${ONYX_TARGETS_FOLDER}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES
            SOVERSION ${MAJOR_VERSION}
            VERSION ${PROJECT_VERSION}
    )
    
    #### Precompiled Header ####
    if (DEFINED TARGET_PRECOMPILED_HEADER)
        target_precompile_headers(${ONYX_TARGET_NAME} PUBLIC ${TARGET_PRECOMPILED_HEADER})
    endif()

    #### Compiler Defines ####
    target_compile_definitions(${ONYX_TARGET_NAME} PUBLIC ${ONYX_PUBLIC_DEFINES})
    target_compile_definitions(${ONYX_TARGET_NAME} PRIVATE ${ONYX_PRIVATE_DEFINES})

    if (DEFINED TARGET_PUBLIC_DEFINES)
        target_compile_definitions(${ONYX_TARGET_NAME} PUBLIC ${TARGET_PUBLIC_DEFINES})
    endif()

     if (DEFINED TARGET_PRIVATE_DEFINES)
        target_compile_definitions(${ONYX_TARGET_NAME} PRIVATE ${TARGET_PRIVATE_DEFINES})
    endif()

    #### Compiler & Link Options ####
    target_compile_options(${ONYX_TARGET_NAME} PRIVATE ${ONYX_COMPILE_OPTIONS})
    target_link_options(${ONYX_TARGET_NAME} PRIVATE ${ONYX_LINK_OPTIONS})

    #### Includes ####
    target_include_directories(${ONYX_TARGET_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
        $<INSTALL_INTERFACE:include>
    )
    target_include_directories(${ONYX_TARGET_NAME} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
        $<INSTALL_INTERFACE:source>
    )

    #### Dependencies ####
    if(DEFINED TARGET_PUBLIC_DEPENDENCIES)
        target_link_libraries(${ONYX_TARGET_NAME} PUBLIC ${TARGET_PUBLIC_DEPENDENCIES})
    endif()

    if(DEFINED TARGET_PRIVATE_DEPENDENCIES)
        target_link_libraries(${ONYX_TARGET_NAME} PRIVATE ${TARGET_PRIVATE_DEPENDENCIES})
    endif()

if (ONYX_ENABLE_INSTALL)
    #### Install ####
    include(GNUInstallDirs)
    install(TARGETS ${ONYX_TARGET_NAME}
        EXPORT ${ONYX_TARGET_NAME}-targets
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

    install(EXPORT 
        ${ONYX_TARGET_NAME}-targets
        DESTINATION ${ONYX_INSTALL_CMAKEDIR}
        NAMESPACE onyx::
        FILE ${ONYX_TARGET_NAME}-targets.cmake
        COMPONENT ${PROJECT_NAME}_Development
    )
endif()

endfunction()
