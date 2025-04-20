set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})	

message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

if (WIN32 AND NOT Vulkan_shaderc_combined_DEBUG_LIBRARY)
    message(INFO " Vulkan shaderc debug library not found. Consider installing it for shader compilation support.")
    message(INFO " Getting shaderc from git instead.")

    set(shaderc_prebuilt_debug_url "https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/windows/continuous_debug_2019/73/20250109-143808/install.zip")

    CPMAddPackage(
        NAME shaderc_debug
        URL ${shaderc_prebuilt_debug_url}
        EXCLUDE_FROM_ALL YES
    )

    unset(shaderc_prebuilt_debug_url)

    if (shaderc_debug_FOUND)
        set_property(TARGET Vulkan::shaderc_combined APPEND
            PROPERTY
                IMPORTED_CONFIGURATIONS Debug)
        set_property(TARGET Vulkan::shaderc_combined
            PROPERTY
            IMPORTED_LOCATION_DEBUG "${shaderc_debug_SOURCE_DIR}/lib/shaderc_combined.lib")
    else()
        message(INFO " Failed getting shaderc debug library from prebuilt git.")
    endif()
endif()

CPMAddPackage(
	NAME VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    VERSION 3.2.0
    OPTIONS "VMA_ENABLE_INSTALL ${ONYX_ENABLE_INSTALL}"
)

set(CMAKE_FOLDER extern/SPIRV-Cross)
CPMAddPackage(SPIRV-Cross
    GITHUB_REPOSITORY KhronosGroup/SPIRV-Cross
    GIT_TAG 82331a2ca9f2ea90d05901e931aa8c21e70dc40c
    OPTIONS
        "SPIRV_CROSS_ENABLE_TESTS OFF"
)

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-assets
	onyx-nodegraph
)

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-core
	onyx-filesystem
    onyx-profiler
	Vulkan::Vulkan
    Vulkan::shaderc_combined
    GPUOpen::VulkanMemoryAllocator
	spirv-cross-core
	spirv-cross-glsl
)

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")