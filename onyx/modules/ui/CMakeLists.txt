if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-ui
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::Ui"
    PUBLIC_DEFINES "ONYX_UI_MODULE=1"
)