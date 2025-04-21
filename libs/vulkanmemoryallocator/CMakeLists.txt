find_package(Vulkan)

CPMAddPackage(
	NAME VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    VERSION 3.2.0
)

add_library(onyx-vma STATIC ${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc.cpp)
set_target_properties(onyx-vma PROPERTIES FOLDER extern/VulkanMemoryAllocator)
target_link_libraries(onyx-vma PRIVATE Vulkan::Vulkan)
target_include_directories(onyx-vma
    PUBLIC
        $<BUILD_INTERFACE:${VulkanMemoryAllocator_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/onyx-vma>
)

install(TARGETS onyx-vma EXPORT onyx-graphics-targets)