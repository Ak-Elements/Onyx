#pragma once
#include <imgui.h>

namespace Onyx::Editor
{
    enum class IconType: onyxU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond };
    void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, bool showTriangleTip, ImU32 color, ImU32 innerColor);
}