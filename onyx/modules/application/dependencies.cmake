message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-entity
	onyx-graphics
	onyx-input
	onyx-nodegraph
	onyx-ui
	onyx-volume 
)

#remove onyx modules that should not be there (e.g. onyx-volume)

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-profiler
)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")
