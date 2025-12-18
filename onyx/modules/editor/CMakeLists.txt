if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

if (NOT ONYX_BUILD_EDITOR)
        return()
endif()

onyx_add_target(onyx-editor
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::Editor"
)
get_property(editor_implementations GLOBAL PROPERTY onyx_generated_editor_implementations)
target_sources(onyx-editor PRIVATE ${editor_implementations})
source_group(TREE ${CMAKE_CURRENT_BINARY_DIR}/generated/private/onyx/editor FILES ${editor_implementations})