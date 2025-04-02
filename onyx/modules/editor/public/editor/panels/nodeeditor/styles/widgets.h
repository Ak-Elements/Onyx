#pragma once
#include <imgui.h>
#include <editor/panels/nodeeditor/styles/drawing.h>

namespace Onyx::Editor
{
    void Icon(const ImVec2& iconSize, const ImVec2& size, IconType type, bool filled, bool showTriangleTip, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));

}
