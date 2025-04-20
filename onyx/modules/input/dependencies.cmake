message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-graphics
)
# maybe move the window out of onyx-graphics to avoid pulling the whole graphics module just for input handling 

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-profiler
)

if (WIN32)

    if (ONYX_USE_GAMEINPUT)
	    find_package(GameInput)

        if (NOT GameInput_FOUND)
            message(WARNING "GameInput not found. Ensure the Windows SDK is installed or specify WINDOWS_SDK_BASE_DIR. Disabling GameInput.")
            set(ONYX_USE_GAMEINPUT FALSE)
        endif()
    endif()

    if(ONYX_USE_GAMEINPUT)
		list(APPEND TARGET_PRIVATE_DEPENDENCIES GameInput::GameInput)
	else()
		list(APPEND TARGET_PRIVATE_DEPENDENCIES XInput)
	endif()

endif()


message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")
