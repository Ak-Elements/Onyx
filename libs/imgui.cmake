# base it on commit because versions are not updated
set(IMGUI_NODE_EDITOR_VERSION e78e447900909a051817a760efe13fe83e6e1afc)
# base it on commit because versions are not updated
set(IMGUIZMO_VERSION b10e91756d32395f5c1fefd417899b657ed7cb88)

#docking and layout branch
CPMAddPackage(
    NAME ImGui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.91.6-docking
    DOWNLOAD_ONLY TRUE
	UPDATE_DISCONNECTED 1
	PATCH_COMMAND git apply "${CMAKE_CURRENT_LIST_DIR}/imgui-stacklayout.patch"
)

CPMAddPackage(
    NAME ImGuizmo
	GITHUB_REPOSITORY CedricGuillemet/ImGuizmo
    DOWNLOAD_ONLY TRUE
	GIT_TAG ${IMGUIZMO_VERSION}
)

CPMAddPackage(
	NAME ImGuiNodeEditor
	GITHUB_REPOSITORY thedmd/imgui-node-editor
	GIT_TAG ${IMGUI_NODE_EDITOR_VERSION}
	DOWNLOAD_ONLY TRUE
	UPDATE_DISCONNECTED 1
	PATCH_COMMAND git apply "${CMAKE_CURRENT_LIST_DIR}/imgui-nodeeditor.patch"
)

CPMAddPackage(
	NAME ImPlot2D
	GITHUB_REPOSITORY epezent/implot
	VERSION 0.16
	DOWNLOAD_ONLY TRUE
)

CPMAddPackage(
	NAME ImPlot3D
	GITHUB_REPOSITORY brenocq/implot3d
	VERSION 0.2
	DOWNLOAD_ONLY TRUE
)

add_library(ImGui STATIC)
set_target_properties (ImGui PROPERTIES FOLDER extern/imgui)

target_sources(ImGui PRIVATE
	${ImGui_SOURCE_DIR}/imgui_demo.cpp
	${ImGui_SOURCE_DIR}/imgui_draw.cpp
	${ImGui_SOURCE_DIR}/imgui_tables.cpp
	${ImGui_SOURCE_DIR}/imgui_widgets.cpp
	${ImGui_SOURCE_DIR}/imgui.cpp
    ${ImGui_SOURCE_DIR}/imgui_stacklayout.cpp
)

target_include_directories(ImGui PUBLIC
    $<BUILD_INTERFACE:${ImGui_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/imgui>
)

target_compile_definitions(ImGui PUBLIC "IMGUI_HAS_STACK_LAYOUT=1")

add_library(ImGuizmo STATIC ${ImGuizmo_SOURCE_DIR}/ImGuizmo.cpp)
set_target_properties(ImGuizmo PROPERTIES FOLDER extern/imgui)
target_include_directories(ImGuizmo PUBLIC
 $<BUILD_INTERFACE:${ImGuizmo_SOURCE_DIR}>
 $<INSTALL_INTERFACE:include/imgui>)
 target_link_libraries(ImGuizmo PRIVATE ImGui)

add_library(ImGuiNodeEditor STATIC)
set_target_properties (ImGuiNodeEditor PROPERTIES FOLDER extern/imgui)
target_sources(ImGuiNodeEditor PRIVATE
	${ImGuiNodeEditor_SOURCE_DIR}/crude_json.cpp
	${ImGuiNodeEditor_SOURCE_DIR}/imgui_canvas.cpp
	${ImGuiNodeEditor_SOURCE_DIR}/imgui_node_editor.cpp
	${ImGuiNodeEditor_SOURCE_DIR}/imgui_node_editor_api.cpp
)

target_include_directories(ImGuiNodeEditor SYSTEM PUBLIC
    $<BUILD_INTERFACE:${ImGuiNodeEditor_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/imgui>)
target_link_libraries(ImGuiNodeEditor PRIVATE ImGui)

add_library(ImGuiPlot2D STATIC)
set_target_properties (ImGuiPlot2D PROPERTIES FOLDER extern/imgui)
target_sources(ImGuiPlot2D PRIVATE
	${ImPlot2D_SOURCE_DIR}/implot.cpp
	${ImPlot2D_SOURCE_DIR}/implot_items.cpp
)

target_include_directories(ImGuiPlot2D PUBLIC
    $<BUILD_INTERFACE:${ImPlot2D_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/imgui>)
target_link_libraries(ImGuiPlot2D PRIVATE ImGui)

add_library(ImGuiPlot3D STATIC)
set_target_properties (ImGuiPlot3D PROPERTIES FOLDER extern/imgui)
target_sources(ImGuiPlot3D PRIVATE
	${ImPlot3D_SOURCE_DIR}/implot3d.cpp
	${ImPlot3D_SOURCE_DIR}/implot3d_items.cpp
)

target_include_directories(ImGuiPlot3D PUBLIC
    $<BUILD_INTERFACE:${ImPlot3D_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/imgui>)
target_link_libraries(ImGuiPlot3D PRIVATE ImGui)

if (ONYX_ENABLE_INSTALL)
    include(GNUInstallDirs)
    install(
        TARGETS
            ImGui
            ImGuizmo
            ImGuiNodeEditor
            ImGuiPlot2D
            ImGuiPlot3D
        EXPORT imgui-targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT ${PROJECT_NAME}_RunTime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_RunTime
            NAMELINK_COMPONENT ${PROJECT_NAME}_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${PROJECT_NAME}_Development
    )

    install(EXPORT imgui-targets
        FILE imgui-targets.cmake
        NAMESPACE imgui::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT imgui_Development
    )

    # Export targets to build tree root
    export(TARGETS
        ImGui
        ImGuizmo
        ImGuiNodeEditor
        ImGuiPlot2D
        ImGuiPlot3D
        NAMESPACE imgui::
        FILE ${CMAKE_BINARY_DIR}/imgui-targets.cmake
    )

endif()