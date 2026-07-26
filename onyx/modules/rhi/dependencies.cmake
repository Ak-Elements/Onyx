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

# SLANG
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(SLANG_VERSION "2026.13.1")
  set(Slang_URL https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-windows-x86_64.tar.gz)
  set(Slang_SHA_256 )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(SLANG_VERSION "2026.13.1")
  set(Slang_URL https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-linux-x86_64.tar.gz)
  set(Slang_SHA_256 c1ed948af94c6fd2034cc0f82f6892b8b287d5362939758cb4955676f26d893e)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
  message("Use find_package(slang) for Android platform")
else ()
  message(FATAL_ERROR "Unsupported platform")
endif ()

if (Slang_URL AND Slang_SHA_256)
  CPMAddPackage(
    NAME slang_download
    VERSION ${SLANG_VERSION}
    URL ${Slang_URL}
    URL_HASH SHA256=${Slang_SHA_256}
    DOWNLOAD_ONLY
  )

  if (slang_download_ADDED)
    set(slang_DIR "${slang_download_SOURCE_DIR}/lib/cmake/slang")
    if(NOT EXISTS ${slang_DIR})
      message(FATAL_ERROR "slang install directory not found: \"${slang_DIR}\"")
    endif()
  
    find_package(slang REQUIRED NO_DEFAULT_PATH)

  else ()
    message(FATAL_ERROR "Unable to add slang from \"${Slang_URL}\"")
  endif ()
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(slang_INSTALL ${slang_BUILD}/android-${ANDROID_ABI})
  set(slang_DIR "${slang_INSTALL}/lib/cmake/slang")
  if(NOT EXISTS ${slang_DIR})
    message(FATAL_ERROR "slang install directory not found: \"${slang_DIR}\"")
  endif()
  

  find_package(slang REQUIRED)
endif ()

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
    slang::slang
)

find_package(glslang CONFIG QUIET)
if (TARGET glslang::glslang)
    list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES glslang::glslang)
endif()

find_package(SPIRV-Tools QUIET)
if (TARGET SPIRV-Tools)
    list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES SPIRV-Tools)
endif()

if (TARGET SPIRV-Tools-opt)
    list(APPEND onyx_TARGET_PRIVATE_DEPENDENCIES SPIRV-Tools-opt)
endif()

set(CMAKE_FOLDER ${CMAKE_FOLDER_PREV})
