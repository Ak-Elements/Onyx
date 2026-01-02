if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

include(add_onyx_module)
include(onyx_generate_module)

option(ONYX_BUILD_EDITOR "Build editor" OFF)

list(APPEND ONYX_PUBLIC_DEFINES
    $<IF:$<BOOL:${ONYX_BUILD_EDITOR}>,ONYX_IS_EDITOR=1,ONYX_IS_EDITOR=0>
)
if (ONYX_BUILD_EDITOR)
    add_library(onyx-editor STATIC)
endif()
set(ONYX_PUBLIC_DEFINES ${ONYX_PUBLIC_DEFINES} PARENT_SCOPE)

# modules
add_subdirectory(profiler)
add_subdirectory(core)
add_subdirectory(filesystem)
add_subdirectory(assets)
add_subdirectory(nodegraph)
add_subdirectory(graphics)
add_subdirectory(entity)
add_subdirectory(input)
add_subdirectory(localization)
add_subdirectory(ui)
add_subdirectory(gamecore)
add_subdirectory(volume)
add_subdirectory(editor)
add_subdirectory(application)