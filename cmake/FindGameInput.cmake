# FindGameInput.cmake - Locate the GameInput API within the Windows SDK

# Function to search for GameInput files in the Windows SDK
function(find_gameinput_files sdk_base_dir)

    # Search all versions of the SDK in the Include directory
    file(GLOB SDK_VERSIONS "${sdk_base_dir}/Include/*")
    list(SORT SDK_VERSIONS ORDER DESCENDING)  # Start from the newest version

    foreach(SDK_VERSION_PATH ${SDK_VERSIONS})
        # Get the version string (e.g., "10.0.19041.0")
        get_filename_component(SDK_VERSION ${SDK_VERSION_PATH} NAME)

        # Locate the gameinput.h header in the specific SDK version
        find_path(GAMEINPUT_INCLUDE_DIR
            NAMES gameinput.h
            PATHS ${sdk_base_dir}/Include/${SDK_VERSION}
            PATH_SUFFIXES um
            NO_DEFAULT_PATH
        )

        # Locate the gameinput.lib library in the specific SDK version
        find_library(GAMEINPUT_LIBRARY
            NAMES gameinput
            PATHS ${sdk_base_dir}/Lib/${SDK_VERSION}
            PATH_SUFFIXES um/x64
            NO_DEFAULT_PATH
        )

        # If both the header and the library are found, stop searching
        if (GAMEINPUT_INCLUDE_DIR AND GAMEINPUT_LIBRARY)
            message(STATUS "Found GameInput in Windows SDK ${SDK_VERSION}")
            set(GameInput_FOUND TRUE PARENT_SCOPE)
            return()  # Exit the function as we've found the files
        endif()
    endforeach()

    set(GAMEINPUT_INCLUDE_DIR "" PARENT_SCOPE)
    set(GAMEINPUT_LIBRARY "" PARENT_SCOPE)
    set(GameInput_FOUND FALSE PARENT_SCOPE)

endfunction()

# Check if the user has provided a specific SDK path
if (NOT DEFINED WINDOWS_SDK_BASE_DIR)
    # If not, use default paths for Windows SDK installation
    set(DEFAULT_SDK_PATHS
        "$ENV{ProgramFiles\(x86\)}/Windows Kits/10"
        "$ENV{ProgramFiles}/Windows Kits/10"
    )
else()
    # Use the user-specified SDK path
    list(APPEND DEFAULT_SDK_PATHS ${WINDOWS_SDK_BASE_DIR})
endif()

# Try to find the Windows SDK
set(GameInput_FOUND FALSE)
foreach(SDK_PATH ${DEFAULT_SDK_PATHS})
    # Check if we can find both the header and library in this SDK path
    find_gameinput_files(${SDK_PATH})
    if (GameInput_FOUND)
        break()
    endif()
endforeach()

set(GameInput_FOUND ${GameInput_FOUND} PARENT_SCOPE)
if (GameInput_FOUND)
    # Create an imported target for GameInput
    add_library(GameInput::GameInput UNKNOWN IMPORTED)
    set_target_properties(GameInput::GameInput PROPERTIES
        IMPORTED_LOCATION "${GAMEINPUT_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GAMEINPUT_INCLUDE_DIR}"
    )
endif()