set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})	

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

if (WIN32 AND NOT Vulkan_shaderc_combined_DEBUG_LIBRARY)
    message(WARNING " Vulkan shaderc debug library not found. Debug build will not be working. Consider installing it for shader compilation support.")
endif()
set(CMAKE_FOLDER extern/SPIRV-Cross)
CPMAddPackage(SPIRV-Cross
    GITHUB_REPOSITORY KhronosGroup/SPIRV-Cross
    GIT_TAG 82331a2ca9f2ea90d05901e931aa8c21e70dc40c
    OPTIONS
        "SPIRV_CROSS_ENABLE_TESTS OFF"
)

set(onyx_TARGET_PUBLIC_DEPENDENCIES
    onyx-platform
    onyx-assets
)

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    onyx-core
    onyx-filesystem
    onyx-profiler
    onyx-vma
    Vulkan::Vulkan
    Vulkan::shaderc_combined
    spirv-cross-core
    spirv-cross-glsl
)

find_package(glslang CONFIG)
if (TARGET glslang::glslang)
    list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES glslang::glslang)
endif()

find_package(SPIRV-Tools)
if (TARGET SPIRV-Tools)
    list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES SPIRV-Tools)
endif()

if (TARGET SPIRV-Tools-opt)
    list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES SPIRV-Tools-opt)
endif()

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})