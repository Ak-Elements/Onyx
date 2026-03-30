if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-core
    NAMESPACE "onyx"
    NAMESPACE_PATH "onyx"
    FOLDER ${ONYX_TARGETS_FOLDER}
    NO_CODEGEN
    NO_TOOLS_TARGET
)