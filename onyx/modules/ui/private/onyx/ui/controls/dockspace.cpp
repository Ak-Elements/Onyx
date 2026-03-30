#include <onyx/ui/controls/dockspace.h>

#include <imgui_internal.h>

namespace onyx::ui {
void Dockspace::init( uint32_t id, const ImGuiWindowClass* windowClass, const DynamicArray< DockSplit >& splits ) {
    m_id = id;
    m_windowClass = windowClass;

    // Setup dockspace
    ImGui::DockBuilderRemoveNode( m_id );

    ImGuiID currentNode = ImGui::DockBuilderAddNode( m_id );
    ImGui::DockBuilderSetNodePos( currentNode, ImVec2( 0, 0 ) );
    ImGui::DockBuilderSetNodeSize( currentNode, ImGui::GetWindowSize() );

    for( const DockSplit& split : splits ) {
        ImGuiDir direction = ImGuiDir_None;
        switch( split.Direction ) {
        case DockSplitDirection::Left:
            direction = ImGuiDir_Left;
            break;
        case DockSplitDirection::Right:
            direction = ImGuiDir_Right;
            break;
        case DockSplitDirection::Up:
            direction = ImGuiDir_Up;
            break;
        case DockSplitDirection::Down:
            direction = ImGuiDir_Down;
            break;
        }

        ImGuiID nodeInDirection, nodeOppositeDirection;
        ImGui::DockBuilderSplitNode( currentNode, direction, split.Ratio, &nodeInDirection, &nodeOppositeDirection );

        if( split.DockWindowInDirection.empty() == false ) {
            ImGui::DockBuilderDockWindow( split.DockWindowInDirection.c_str(), nodeInDirection );
        }

        if( split.DockWindowOppositeDirection.empty() == false ) {
            ImGui::DockBuilderDockWindow( split.DockWindowOppositeDirection.c_str(), nodeOppositeDirection );
        }

        currentNode = nodeInDirection;
    }

    ImGui::DockBuilderFinish( m_id );
}

void Dockspace::render() {
    if( m_id == 0 )
        return;

    ImGui::DockSpace( m_id, ImVec2( 0, 0 ), ImGuiDockNodeFlags_PassthruCentralNode, m_windowClass );
}

} // namespace onyx::ui
