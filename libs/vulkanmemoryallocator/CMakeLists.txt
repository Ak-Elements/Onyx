find_package(Vulkan)

CPMAddPackage(
	NAME VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    VERSION 3.2.0
    SYSTEM ON
    OPTIONS
        "VMA_ENABLE_INSTALL ${ONYX_ENABLE_INSTALL}"
)

add_library(onyx-vma STATIC ${CMAKE_CURRENT_SOURCE_DIR}/vulkanmemoryallocator.cpp)
set_target_properties(onyx-vma PROPERTIES FOLDER extern/VulkanMemoryAllocator)
target_link_libraries(onyx-vma PRIVATE Vulkan::Vulkan)
target_link_libraries(onyx-vma PUBLIC GPUOpen::VulkanMemoryAllocator)

target_include_directories(onyx-vma
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:include/onyx-vma>
)

install(TARGETS onyx-vma EXPORT onyx-graphics-targets)