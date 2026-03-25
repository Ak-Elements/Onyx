#pragma once
#include <imgui.h>

namespace onyx::editor {
enum class IconType : uint32_t { Flow, Circle, Square, Grid, RoundSquare, Diamond };
void DrawIcon( ImDrawList* drawList,
               const ImVec2& a,
               const ImVec2& b,
               IconType type,
               bool filled,
               bool showTriangleTip,
               ImU32 color,
               ImU32 innerColor );
} // namespace onyx::editor