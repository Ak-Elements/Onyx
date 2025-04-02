message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

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