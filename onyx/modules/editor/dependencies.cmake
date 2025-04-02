message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-gamecore
	onyx-input
	onyx-volume
	ImGui
    ImGuiNodeEditor
	ImGuiPlot2D
	ImGuiPlot3D
)

set(TARGET_PRIVATE_DEPENDENCIES

)
message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")
