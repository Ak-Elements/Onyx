if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-input
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::Input"
    NO_EDITOR_TARGET
)