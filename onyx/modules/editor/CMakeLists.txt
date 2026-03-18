if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

if (NOT ONYX_BUILD_EDITOR)
        return()
endif()

onyx_add_target(onyx-editor
    NAMESPACE "onyx::editor"
    NAMESPACE_PATH "onyx/editor"
    FOLDER ${ONYX_TARGETS_FOLDER}
    NO_EDITOR_TARGET
)