if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-platform
    NAMESPACE "onyx::platform"
    NAMESPACE_PATH "onyx/platform"
    FOLDER ${ONYX_TARGETS_FOLDER}
    NO_TOOLS_TARGET
)