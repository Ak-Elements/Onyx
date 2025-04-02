#define IMGUI_DEFINE_MATH_OPERATORS
#include <editor/panels/nodeeditor/styles/widgets.h>
#include <editor/panels/nodeeditor/styles/drawing.h>

void Onyx::Editor::Icon(const ImVec2& iconSize, const ImVec2& size, IconType type, bool filled, bool showTriangleTip, const ImVec4& color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& innerColor/* = ImVec4(0, 0, 0, 0)*/)
{
    if (ImGui::IsRectVisible(iconSize))
    {
        const ImVec2& cursorPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        DrawIcon(drawList, cursorPos, cursorPos + iconSize, type, filled, showTriangleTip, ImColor(color), ImColor(innerColor));
    }

    const ImVec2& cursorPos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(cursorPos + (iconSize / 2.0f) - (size / 2.0f));
    ImGui::Dummy(size);
}

