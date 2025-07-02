#include <editor/modules/nodeeditor.h>

#include <onyx/ui/widgets.h>
#include <imgui_node_editor.h>

namespace Onyx::NodeGraph
{
    namespace
    {
        constexpr onyxF32 ICON_SIZE = 24.0f;
    }
    void DrawPlusIconBackground(Guid64 id)
    {
        ImVec2 position = ax::NodeEditor::GetNodePosition(id.Get());
        ImVec2 size = ax::NodeEditor::GetNodeSize(id.Get());
        auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList(id.Get());
        if (drawlist)
        {
            ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
            Ui::DrawPlusIcon(drawlist, offset, ICON_SIZE, 0x33FFFFFF);
        } 
    }

    void DrawMinusIconBackground(Guid64 id)
    {
        ImVec2 position = ax::NodeEditor::GetNodePosition(id.Get());
        ImVec2 size = ax::NodeEditor::GetNodeSize(id.Get());
        auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList(id.Get());
        if (drawlist)
        {
            ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
            Ui::DrawMinusIcon(drawlist, offset, ICON_SIZE, 0x33FFFFFF);
        }
    }

    void DrawMultiplyIconBackground(Guid64 id)
    {
        ImVec2 position = ax::NodeEditor::GetNodePosition(id.Get());
        ImVec2 size = ax::NodeEditor::GetNodeSize(id.Get());
        auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList(id.Get());
        if (drawlist)
        {
            ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
            Ui::DrawXIcon(drawlist, offset, ICON_SIZE, 0x33FFFFFF);
        }
    }

    void DrawDivisionIconBackground(Guid64 id)
    {
        ImVec2 position = ax::NodeEditor::GetNodePosition(id.Get());
        ImVec2 size = ax::NodeEditor::GetNodeSize(id.Get());
        auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList(id.Get());
        if (drawlist)
        {
            ImVec2 offset = ImGui::GetCursorScreenPos() - ImVec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
            Ui::DrawDivisionIcon(drawlist, offset, ICON_SIZE, 0x33FFFFFF);
        }
    }

    void DrawAbsoluteIconBackground(Guid64 id)
    {
        ImVec2 position = ax::NodeEditor::GetNodePosition(id.Get());
        ImVec2 size = ax::NodeEditor::GetNodeSize(id.Get());
        auto drawlist = ax::NodeEditor::GetNodeBackgroundDrawList(id.Get());
        if (drawlist)
        {
            ImVec2 center(position.x + size.x * 0.5f, position.y + size.y * 0.5f);

            constexpr onyxF32 iconHalfSize = ICON_SIZE * 0.5f;
            constexpr onyxF32 thickness = ICON_SIZE * 0.1f;

            ImVec2 min = center - ImVec2(iconHalfSize, -iconHalfSize * 0.85f);
            ImVec2 max = center - ImVec2(iconHalfSize - thickness, iconHalfSize * 0.85f);

            drawlist->AddRectFilled(min, max, 0x33FFFFFF);
            Ui::DrawXIcon(drawlist, ImGui::GetCursorScreenPos() - center, iconHalfSize, 0x33FFFFFF);

            min = center + ImVec2(iconHalfSize - thickness, -iconHalfSize * 0.85f);
            max = center + ImVec2(iconHalfSize, iconHalfSize * 0.85f);
            drawlist->AddRectFilled(min, max, 0x33FFFFFF);
        }
    }
}
