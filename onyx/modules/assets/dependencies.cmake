message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PRIVATE_DEPENDENCIES
	onyx-core
	onyx-filesystem
)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")