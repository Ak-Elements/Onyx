﻿if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

#### Target ####
set(CURRENT_TARGET onyx-graphics)
include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/source_definitions.cmake)

set (onyx-graphics_PRIVATE_DEFINES
    ONYX_USE_VULKAN
    $<$<BOOL:${USE_SDL2}>:ONYX_USE_SDL2>
    $<$<CXX_COMPILER_ID:Clang>:>
	$<$<CXX_COMPILER_ID:AppleClang>:>
	$<$<CXX_COMPILER_ID:GNU>:>
)

if (WIN32)
    list(APPEND onyx-graphics_PRIVATE_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

add_onyx_module(${CURRENT_TARGET}
    PUBLIC_SOURCES ${TARGET_PUBLIC_SOURCES}
    PRIVATE_SOURCES ${TARGET_PRIVATE_SOURCES}
    PUBLIC_DEPENDENCIES ${TARGET_PUBLIC_DEPENDENCIES}
    PRIVATE_DEPENDENCIES ${TARGET_PRIVATE_DEPENDENCIES}
    PRIVATE_DEFINES ${onyx-graphics_PRIVATE_DEFINES}
)