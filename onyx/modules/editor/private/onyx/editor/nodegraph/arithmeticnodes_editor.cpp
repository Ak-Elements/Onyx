#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>

#include <imgui_node_editor.h>
#include <onyx/ui/widgets.h>

namespace onyx::node_graph {
namespace {
constexpr float32 IconSize = 24.0f;
}
void drawPlusIconBackground( Guid64 id ) {
    ImVec2 position = ax::NodeEditor::GetNodePosition( id.get() );
    ImVec2 size = ax::NodeEditor::GetNodeSize( id.get() );
    auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList( id.get() );
    if( drawlist ) {
        ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2( position.x + size.x * 0.5f, position.y + size.y * 0.5f );
        ui::drawPlusIcon( drawlist, offset, IconSize, 0x33FFFFFF );
    }
}

void drawMinusIconBackground( Guid64 id ) {
    ImVec2 position = ax::NodeEditor::GetNodePosition( id.get() );
    ImVec2 size = ax::NodeEditor::GetNodeSize( id.get() );
    auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList( id.get() );
    if( drawlist ) {
        ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2( position.x + size.x * 0.5f, position.y + size.y * 0.5f );
        ui::drawMinusIcon( drawlist, offset, IconSize, 0x33FFFFFF );
    }
}

void drawMultiplyIconBackground( Guid64 id ) {
    ImVec2 position = ax::NodeEditor::GetNodePosition( id.get() );
    ImVec2 size = ax::NodeEditor::GetNodeSize( id.get() );
    auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList( id.get() );
    if( drawlist ) {
        ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2( position.x + size.x * 0.5f, position.y + size.y * 0.5f );
        ui::drawXIcon( drawlist, offset, IconSize, 0x33FFFFFF );
    }
}

void drawDivisionIconBackground( Guid64 id ) {
    ImVec2 position = ax::NodeEditor::GetNodePosition( id.get() );
    ImVec2 size = ax::NodeEditor::GetNodeSize( id.get() );
    auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList( id.get() );
    if( drawlist ) {
        ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2( position.x + size.x * 0.5f, position.y + size.y * 0.5f );
        ui::drawDivisionIcon( drawlist, offset, IconSize, 0x33FFFFFF );
    }
}

void drawAbsoluteIconBackground( Guid64 id ) {
    ImVec2 position = ax::NodeEditor::GetNodePosition( id.get() );
    ImVec2 size = ax::NodeEditor::GetNodeSize( id.get() );
    auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList( id.get() );
    if( drawlist ) {
        ImVec2 center( position.x + size.x * 0.5f, position.y + size.y * 0.5f );

        constexpr float32 IconHalfSize = IconSize * 0.5f;
        constexpr float32 Thickness = IconSize * 0.1f;

        ImVec2 min = center - ImVec2( IconHalfSize, -IconHalfSize * 0.85f );
        ImVec2 max = center - ImVec2( IconHalfSize - Thickness, IconHalfSize * 0.85f );

        drawlist->AddRectFilled( min, max, 0x33FFFFFF );
        ui::drawXIcon( drawlist, ImGui::GetCursorScreenPos() - center, IconHalfSize, 0x33FFFFFF );

        min = center + ImVec2( IconHalfSize - Thickness, -IconHalfSize * 0.85f );
        max = center + ImVec2( IconHalfSize, IconHalfSize * 0.85f );
        drawlist->AddRectFilled( min, max, 0x33FFFFFF );
    }
}
} // namespace onyx::node_graph
