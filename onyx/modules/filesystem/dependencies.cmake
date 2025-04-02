unset(TARGET_PUBLIC_DEPENDENCIES)
unset(TARGET_PRIVATE_DEPENDENCIES)

message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(STB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/stb-src)

CPMAddPackage(
	NAME stb
  	GITHUB_REPOSITORY nothings/stb
  	GIT_TAG master
  	DOWNLOAD_ONLY YES
	SOURCE_DIR ${STB_INCLUDE_DIR}/stb
  )

if (stb_ADDED)
	# stb has no CMake support, so we create our own target

	add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE
        $<BUILD_INTERFACE:${STB_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
endif()

CPMAddPackage(
	NAME nlohmann_json
	VERSION 3.11.3
	GITHUB_REPOSITORY nlohmann/json
	OPTIONS
    	"JSON_BuildTests OFF"
	EXCLUDE_FROM_ALL TRUE
)

CPMAddPackage(
	NAME efsw
	GITHUB_REPOSITORY SpartanJ/efsw
	GIT_TAG 1.4.0
	EXCLUDE_FROM_ALL TRUE
)

CPMAddPackage(
	NAME nfde
	GITHUB_REPOSITORY btzy/nativefiledialog-extended
	GIT_TAG 29e3bcb578345b9fa345d1d7683f00c150565ca3
	EXCLUDE_FROM_ALL TRUE
    OPTIONS
        "NFD_INSTALL ${ONYX_ENABLE_INSTALL}"
)

if (ONYX_ENABLE_INSTALL)
    install(TARGETS 
        nlohmann_json
        stb_image
        efsw-static
        nfd
        EXPORT onyx-filesystem-targets
    )
endif()

set_target_properties(nfd PROPERTIES FOLDER extern/nfd)
set_target_properties(efsw PROPERTIES FOLDER extern/efsw)
set_target_properties(efsw-static PROPERTIES FOLDER extern/efsw)

set(TARGET_PUBLIC_DEPENDENCIES
	nlohmann_json
)

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-core
	stb_image
	efsw-static
	nfd
)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")
