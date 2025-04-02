message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

FetchContent_Declare(entt
	GIT_REPOSITORY https://github.com/skypjack/entt.git
	GIT_TAG v3.13.2    
)
FetchContent_MakeAvailable(entt)

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-graphics
	EnTT::EnTT
)

# entt should be a private dependency

set(TARGET_PRIVATE_DEPENDENCIES

)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")