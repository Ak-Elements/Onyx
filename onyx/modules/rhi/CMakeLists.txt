if(NOT IS_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(FATAL_ERROR "Please build using the outermost CMakeLists.txt file.")
endif()

set(private_defines
    ONYX_USE_VULKAN
    $<$<BOOL:${USE_SDL2}>:ONYX_USE_SDL2>
    $<$<CXX_COMPILER_ID:Clang>:>
	$<$<CXX_COMPILER_ID:AppleClang>:>
	$<$<CXX_COMPILER_ID:GNU>:>
)

if (WIN32)
    list(APPEND private_defines VK_USE_PLATFORM_WIN32_KHR "ONYX_VK_SURFACE_EXTENSION_NAME=VK_KHR_WIN32_SURFACE_EXTENSION_NAME")
elseif(UNIX)
    list(APPEND private_defines VK_USE_PLATFORM_WAYLAND_KHR "ONYX_VK_SURFACE_EXTENSION_NAME=VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME")
endif()

onyx_add_target(onyx-rhi
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::Rhi"
    PRIVATE_DEFINES ${private_defines}
    NO_EDITOR_TARGET
)
