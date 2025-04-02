message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-profiler
)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")