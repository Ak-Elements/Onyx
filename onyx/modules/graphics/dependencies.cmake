set(CMAKE_FOLDER_PREV, ${CMAKE_FOLDER})	

message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

if (NOT Vulkan_shaderc_combined_FOUND)
    message(WARNING "Vulkan shaderc not found. Consider installing it for shader compilation support.")
    message(WARNING "Getting shaderc from git instead.")

    CPMAddPackage(
        NAME SPIRV-Headers
        GITHUB_REPOSITORY KhronosGroup/SPIRV-Headers
        GIT_TAG 54a521dd130ae1b2f38fef79b09515702d135bdd
        EXCLUDE_FROM_ALL YES
    )

    CPMAddPackage(
        NAME SPIRV-Tools
        GITHUB_REPOSITORY KhronosGroup/SPIRV-Tools
        GIT_TAG f289d047f49fb60488301ec62bafab85573668cc
        OPTIONS
            SPIRV_SKIP_EXECUTABLES ON
            SPIRV_SKIP_TESTS ON
        EXCLUDE_FROM_ALL YES
    )

    CPMAddPackage(
        NAME glslang
        GITHUB_REPOSITORY KhronosGroup/glslang
        GIT_TAG 8b822ee8ac2c3e52926820f46ad858532a895951
        EXCLUDE_FROM_ALL YES
    )

    CPMAddPackage(
        NAME shaderc
        GITHUB_REPOSITORY google/shaderc
        GIT_TAG 0968768c61d4eb7dd861114412e904bb3d59b7b6
        OPTIONS 
            SHADERC_SKIP_TESTS ON
        EXCLUDE_FROM_ALL YES
    )

    target_include_directories(shaderc_combined
    PUBLIC
        $<BUILD_INTERFACE:"${shaderc_SOURCE_DIR}/libshaderc/include">
        $<INSTALL_INTERFACE:include/shaderc>
    )

    add_library(Vulkan::shaderc_combined ALIAS shaderc_combined)

    install(TARGETS shaderc_combined EXPORT onyx-graphics-targets)
    
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
	Vulkan::shaderc_combined
	onyx-profiler
)

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")