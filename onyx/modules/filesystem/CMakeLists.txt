if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-filesystem
    NO_TOOLS_TARGET
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "onyx::file_system"
    NAMESPACE_PATH "onyx/filesystem"
    NO_CODEGEN
    NO_TOOLS_TARGET
)