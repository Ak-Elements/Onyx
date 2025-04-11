set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})	

message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

if (NOT Vulkan_shaderc_combined_FOUND)
    message(INFO " Vulkan shaderc not found. Consider installing it for shader compilation support.")
    message(INFO " Getting shaderc from git instead.")

    CPMAddPackage(
        NAME shaderc_debug
        URL https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/windows/continuous_debug_2019/73/20250109-143808/install.zip
        EXCLUDE_FROM_ALL YES
    )
    
    add_library(shaderc_combinedd STATIC IMPORTED)
    set_target_properties(shaderc_combinedd PROPERTIES
        IMPORTED_LOCATION "${shaderc_debug_SOURCE_DIR}/lib/shaderc_combined.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${shaderc_debug_SOURCE_DIR}/include"
    )

    set(Vulkan_shaderc_combined_DEBUG_LIBRARY shaderc_combinedd)
    
endif()

CPMAddPackage(
	NAME VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    VERSION 3.2.0
)

set(CMAKE_FOLDER extern/SPIRV-Cross)
CPMAddPackage(SPIRV-Cross
    GITHUB_REPOSITORY KhronosGroup/SPIRV-Cross
    GIT_TAG 82331a2ca9f2ea90d05901e931aa8c21e70dc40c
    OPTIONS
        "SPIRV_CROSS_ENABLE_TESTS OFF"
)

add_library(onyx-vma STATIC
    ${VulkanMemoryAllocator_SOURCE_DIR}/src/VmaUsage.cpp
    ${VulkanMemoryAllocator_SOURCE_DIR}/src/VmaUsage.h
    ${VulkanMemoryAllocator_SOURCE_DIR}/include/vk_mem_alloc.h)
set_target_properties(onyx-vma PROPERTIES FOLDER extern/VulkanMemoryAllocator)
target_link_libraries(onyx-vma PRIVATE Vulkan::Vulkan)
target_include_directories(onyx-vma
    PUBLIC
        $<BUILD_INTERFACE:${VulkanMemoryAllocator_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/onyx-vma>
)

install(TARGETS onyx-vma EXPORT onyx-graphics-targets)

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-assets
	onyx-nodegraph
)

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-core
	onyx-filesystem
	onyx-vma
	Vulkan::Vulkan
	spirv-cross-core
	spirv-cross-glsl
    debug ${Vulkan_shaderc_combined_DEBUG_LIBRARY} optimized ${Vulkan_shaderc_combined_LIBRARY}
	onyx-profiler
)

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")