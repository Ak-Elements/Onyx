CPMAddPackage(entt
    GITHUB_REPOSITORY skypjack/entt
    VERSION 3.13.2
    SYSTEM ON
)

set(onyx_TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	EnTT::EnTT
)

# entt should be a private dependency
set(onyx_TARGET_PRIVATE_DEPENDENCIES
)