message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-graphics
)

# maybe move the window out of onyx-graphics to avoid pulling the whole graphics module just for input handling 

if (ONYX_USE_GAMEINPUT)
	find_package(GameInput)

	if (NOT GameInput_FOUND)
    	message(WARNING "GameInput not found. Ensure the Windows SDK is installed or specify WINDOWS_SDK_BASE_DIR. Disabling GameInput.")
		set(ONYX_USE_GAMEINPUT FALSE)
	endif()

endif()

set(TARGET_PRIVATE_DEPENDENCIES
	$<$<BOOL:${ONYX_USE_GAMEINPUT}>:GameInput::GameInput>
	$<$<NOT:$<BOOL:${ONYX_USE_GAMEINPUT}>>:XInput>
	onyx-profiler
)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")
