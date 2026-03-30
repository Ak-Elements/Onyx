if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-ui
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "onyx::ui"
    NAMESPACE_PATH "onyx/ui"
    PUBLIC_DEFINES "ONYX_UI_MODULE=1"
    NO_TOOLS_TARGET
)