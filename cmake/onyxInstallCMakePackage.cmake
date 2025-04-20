include(CMakePackageConfigHelpers)

get_property(onyx_all_components GLOBAL PROPERTY onyx_modules)
if (REMOVE_DUPLICATES)
    list(REMOVE_DUPLICATES onyx_all_components)
endif()

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/onyx-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${ONYX_INSTALL_CMAKEDIR}/onyx-config.cmake"
    @ONLY
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/onyx-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/onyx-config.cmake"
    @ONLY
)


write_basic_package_version_file(
    onyx-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/onyx-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/onyx-config-version.cmake
    DESTINATION ${ONYX_INSTALL_CMAKEDIR}
    COMPONENT ${PROJECT_NAME}_Development
)
