message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

#CLANG-CL We have to clear the system include flags on clang-cl because system includes are buggy
if (WIN32 AND (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    #set(cached_CMAKE_INCLUDE_SYSTEM_FLAG_CXX ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX})
    unset(CMAKE_INCLUDE_SYSTEM_FLAG_CXX)
endif()

CPMAddPackage(
	NAME tracy
	GITHUB_REPOSITORY wolfpld/tracy
	VERSION 0.11.1
)

set_target_properties(TracyClient PROPERTIES FOLDER extern/tracy)

set(CMAKE_FOLDER extern/tracy)
#add_subdirectory(${tracy_SOURCE_DIR}/profiler ${CMAKE_CURRENT_BINARY_DIR}/tracy/profiler)

set(TARGET_PUBLIC_DEPENDENCIES
	TracyClient
	#TracyServer
)

set(TARGET_PRIVATE_DEPENDENCIES
)

message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")

#CLANG-CL We have to clear the system include flags on clang-cl because system includes are buggy
if (WIN32 AND (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    #set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX ${cached_CMAKE_INCLUDE_SYSTEM_FLAG_CXX})
endif()