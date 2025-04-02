set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})	

message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

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
	Vulkan::shaderc_combined
	onyx-profiler
)

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")