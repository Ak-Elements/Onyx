message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-entity
	onyx-graphics
	onyx-nodegraph
	onyx-ui
)

set(TARGET_PRIVATE_DEPENDENCIES

)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")