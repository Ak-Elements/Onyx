CPMAddPackage(Jolt
    GITHUB_REPOSITORY jrouwe/JoltPhysics
    VERSION 5.5.0
    SOURCE_SUBDIR "Build"
)

set(onyx_TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
)

set(onyx_TARGET_PRIVATE_DEPENDENCIES
    Jolt
)