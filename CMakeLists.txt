# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CMakeDependentOption)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 4)
set(PATCH_VERSION 0)
set(ONYX_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
set(ONYX_VERSION_SUFFIX "${MAJOR_VERSION}.${MINOR_VERSION}")

project (
    onyx
    VERSION ${ONYX_VERSION}
    LANGUAGES CXX C
)

if (NOT onyx_SOURCE_DIR)
    set(onyx_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (NOT onyx_BINARY_DIR)
    set(onyx_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Bool Options
option(ONYX_ENABLE_INSTALL "Install onyx components" ON)
option(ONYX_BUILD_ALL "Build all" OFF)
option(ONYX_BUILD_TESTS "Build tests" OFF)
option(ONYX_STATIC_ANALYSIS "Turn static analysis on/off" OFF)
option(ONYX_GENERATE_DATA_SYMLINK "Turn static analysis on/off" OFF)

cmake_dependent_option(ONYX_GENERATE_APP_CONFIG "Generate default application config" OFF "PROJECT_IS_TOP_LEVEL" ON)

# String Options
set(ONYX_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/onyx-${ONYX_VERSION_SUFFIX} CACHE STRING "Path to onyx cmake files")
set(ONYX_PROJECT_DATA_DIR "${CMAKE_SOURCE_DIR}/data" CACHE STRING "Path to the projects data directory")
set(ONYX_PROJECT_TMP_DIR "${CMAKE_SOURCE_DIR}/tmp" CACHE STRING "Path to the projects tmp directory")
set(ONYX_TARGETS_FOLDER "modules" CACHE STRING "Folder for all onyx targets")
set(ONYX_COMPONENT_DEFINITION_FILE_EXTENSION "ocd" CACHE STRING "File extension used for ECS component definitions" FORCE)

set(_ONYX_CODEGEN "${CMAKE_SOURCE_DIR}/tools/codegeneration/out/onyx-codegen")
if(WIN32)
    set(_ONYX_CODEGEN "${_ONYX_CODEGEN}.exe")
endif()

set(ONYX_CODEGEN "${_ONYX_CODEGEN}" CACHE STRING "Path to code generation tool for onyx")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;")

if (CMAKE_COMPILER_IS_GNUCXX OR ((${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") AND NOT MSVC))
    set( ONYX_COMPILE_OPTIONS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -fno-rtti -Wno-comment>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>: >
    $<$<CXX_COMPILER_ID:GNU>: -Wno-changes-meaning> CACHE STRING "Onyx target compile options")
    set( ONYX_LINK_OPTIONS
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: > CACHE STRING "Onyx target link options")

elseif (MSVC)
    set( ONYX_COMPILE_OPTIONS /W4 /WX /Zc:__cplusplus /Zi /GR- CACHE STRING "Onyx target compile options")
    set( ONYX_LINK_OPTIONS /ignore:4099 CACHE STRING "Onyx target link options")
endif()

set( ONYX_PUBLIC_DEFINES 
    $<IF:$<CONFIG:Debug>,ONYX_IS_DEBUG=1,ONYX_IS_DEBUG=0>
    $<IF:$<CONFIG:Release>,ONYX_IS_RELEASE=1,ONYX_IS_RELEASE=0>
    $<IF:$<CONFIG:Retail>,ONYX_IS_RETAIL=1,ONYX_IS_RETAIL=0>
    $<IF:$<CXX_COMPILER_ID:MSVC>,ONYX_IS_MSVC=1,ONYX_IS_MSVC=0>
    $<IF:$<CXX_COMPILER_ID:Clang>,ONYX_IS_CLANG=1,ONYX_IS_CLANG=0>
    $<IF:$<CXX_COMPILER_ID:GNU>,ONYX_IS_GCC=1,ONYX_IS_GCC=0>
    $<IF:$<PLATFORM_ID:Linux>,ONYX_IS_LINUX=1,ONYX_IS_LINUX=0>
)

set( ONYX_PRIVATE_DEFINES
    $<$<CXX_COMPILER_ID:Clang>:>
    $<$<CXX_COMPILER_ID:AppleClang>:>
    $<$<CXX_COMPILER_ID:GNU>:>
    $<$<CXX_COMPILER_ID:MSVC>:>
)

if(UNIX)
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_UNIX=1")
else()
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_UNIX=0")
endif()

if(WIN32)
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_WINDOWS=1")
    list(APPEND ONYX_PRIVATE_DEFINES WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_WINDOWS=0")
endif()

if(APPLE)
    if(IOS)
        list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_IOS=1" "ONYX_IS_MACOS=0")
    else()
        list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_IOS=0" "ONYX_IS_MACOS=1")
    endif()
else()
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_IOS=0" "ONYX_IS_MACOS=0")
endif()

if(ANDROID)
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_ANDROID=1")
else()
    list(APPEND ONYX_PUBLIC_DEFINES "ONYX_IS_ANDROID=0")
endif()

# Remove 'lib' prefix for shared libraries on Windows
# Add multi processor compile on Windows
if (WIN32)
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")
    endif()
    
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

if (NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

if (ONYX_BUILD_ALL)
    set(ONYX_BUILD_TESTS ON)
    set(ONYX_BUILD_SAMPLES ON)
endif()

if (STATIC_ANALYSIS)
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
    if (CMAKE_CXX_CPPCHECK)
        list(
            APPEND CMAKE_CXX_CPPCHECK 
                "--enable=warning"
                "--inconclusive"
                "--force" 
                "--inline-suppr"
                "--error-exitcode=1"
                "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
        )
    endif()
endif()

#CLANG-CL We have to clear the system include flags on clang-cl because system includes are buggy
if (WIN32 AND (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED YES)
endif()

include(FetchContent)
include(cmakepackagemanager)

include(GNUInstallDirs)

execute_process(
    COMMAND git branch --show-current
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE onyx_git_branch
)
string(STRIP "${onyx_git_branch}" onyx_git_branch)

set(config_path "${CMAKE_CURRENT_SOURCE_DIR}/config/${onyx_git_branch}.cmake")
if(NOT ${onyx_git_branch} STREQUAL "main")
    if (EXISTS "${config_path}")
        include("${config_path}")
    else()
        include("config/dev.cmake")
    endif()
else()
    include("config/main.cmake")
endif()

# Get code-generation tool
CPMAddPackage(
    NAME Onyx-CodeGen
    GITHUB_REPOSITORY Ak-Elements/Onyx-CodeGeneration
    GIT_TAG ${ONYX_CODE_GEN_VERSION}
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools/codegeneration/core
    DOWNLOAD_ONLY YES
)

add_subdirectory(libs)
add_subdirectory(onyx)

if (ONYX_ENABLE_INSTALL)
    include(onyxInstallCMakePackage)
endif()

if (NOT PROJECT_IS_TOP_LEVEL)
    
    if (ONYX_GENERATE_APP_CONFIG AND TARGET onyx-application AND NOT EXISTS "${ONYX_PROJECT_DATA_DIR}/appconfig.oconf")

        cmake_path(SET ONYX_ENGINE_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
        cmake_path(
            RELATIVE_PATH ONYX_ENGINE_DATA_DIR
            BASE_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE ONYX_ENGINE_DATA_DIR_RELATIVE
        )

        cmake_path(
            RELATIVE_PATH ONYX_PROJECT_DATA_DIR
            BASE_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE ONYX_PROJECT_DATA_DIR_RELATIVE
        )

        cmake_path(
            RELATIVE_PATH ONYX_PROJECT_TMP_DIR
            BASE_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE ONYX_PROJECT_TMP_DIR_RELATIVE
        )

        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/appconfig.oconf.in" "${ONYX_PROJECT_DATA_DIR}/appconfig.oconf")
    endif()

    if (ONYX_GENERATE_DATA_SYMLINK)
        message(INFO "${${PROJECT_NAME}_SOURCE_DIR}/data")
        file(CREATE_LINK "${${PROJECT_NAME}_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/onyx" RESULT result SYMBOLIC)
    endif()

endif()
