message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

CPMAddPackage(
    NAME utf8cpp
    GITHUB_REPOSITORY nemtrif/utfcpp
    VERSION 4.0.5
)

CPMAddPackage(
    NAME magic_enum
    GITHUB_REPOSITORY Neargye/magic_enum
    VERSION 0.9.5
)

if (ONYX_ENABLE_INSTALL)
    install(TARGETS utf8cpp magic_enum EXPORT onyx-core-targets)
endif()

find_package(Threads)

set(TARGET_PUBLIC_DEPENDENCIES
	utf8cpp
	magic_enum
	onyx-profiler
	${CMAKE_THREAD_LIBS_INIT}
)

message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")

	