if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})

option(ONYX_USE_IMGUI "Use ImGui" ON)

list(APPEND ONYX_PUBLIC_DEFINES
    $<IF:$<BOOL:${ONYX_USE_IMGUI}>,ONYX_USE_IMGUI=1,ONYX_USE_IMGUI=0>
)
set(ONYX_PUBLIC_DEFINES ${ONYX_PUBLIC_DEFINES} PARENT_SCOPE)

if (ONYX_USE_IMGUI)
	include(imgui.cmake)
endif()

add_subdirectory(vulkanmemoryallocator)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})