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
    list(APPEND private_defines VK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX)
    list(APPEND private_defines VK_USE_PLATFORM_WAYLAND_KHR)
endif()

onyx_add_target(onyx-graphicscore
    FOLDER ${ONYX_TARGETS_FOLDER}
    NAMESPACE "Onyx::GraphicsCore"
    PRIVATE_DEFINES ${private_defines}
    NO_EDITOR_TARGET
)
