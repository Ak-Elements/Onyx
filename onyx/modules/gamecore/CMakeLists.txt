if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

onyx_add_target(onyx-gamecore
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::GameCore"
)