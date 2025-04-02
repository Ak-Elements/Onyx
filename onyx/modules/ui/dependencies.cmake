message(STATUS "[${CURRENT_TARGET}] Getting dependencies.")

set(TARGET_PUBLIC_DEPENDENCIES
	onyx-core
	onyx-assets
	onyx-filesystem
	onyx-graphics
	onyx-input
	onyx-profiler
	ImGui
    ImGuizmo
    ImGuiNodeEditor
	ImGuiPlot2D
	ImGuiPlot3D
)

message(STATUS "[${CURRENT_TARGET}] Finished getting dependencies.")
