message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

CPMAddPackage(entt
    GITHUB_REPOSITORY skypjack/entt
    VERSION 3.13.2
    SYSTEM ON
)

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