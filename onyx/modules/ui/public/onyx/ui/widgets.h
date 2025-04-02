#pragma once

#if ONYX_USE_IMGUI

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>

struct ImDrawList;
struct ImVec2;

namespace Onyx::Ui
{
    //TODO maybe move to a common gui file?
    template <typename T>
    static constexpr ImGuiDataType GetImGuiDataType()
    {
        if constexpr (std::is_same_v<T, onyxS8>)
            return ImGuiDataType_S8;
        else if constexpr (std::is_same_v<T, onyxS16>)
            return ImGuiDataType_S16;
        else if constexpr (std::is_same_v<T, onyxS32>)
            return ImGuiDataType_S32;
        else if constexpr (std::is_same_v<T, onyxS64>)
            return ImGuiDataType_S64;
        else if constexpr (std::is_same_v<T, onyxU8>)
            return ImGuiDataType_U8;
        else if constexpr (std::is_same_v<T, onyxU16>)
            return ImGuiDataType_U16;
        else if constexpr (std::is_same_v<T, onyxU32>)
            return ImGuiDataType_U32;
        else if constexpr (std::is_same_v<T, onyxU64>)
            return ImGuiDataType_U64;
        else if constexpr (std::is_same_v<T, onyxF32>)
            return ImGuiDataType_Float;
        else if constexpr (std::is_same_v<T, onyxF64>)
            return ImGuiDataType_Double;

        return ImGuiDataType_COUNT;
    }

    void DrawItemBackground(onyxF32 rounding, onyxF32 borderThickness, onyxU32 color);
    void DrawItemBorder(onyxF32 thickness, onyxF32 rounding, onyxU32 color);

    bool DrawSearchBar(String& searchString, const StringView& hintLabel, bool& grabFocus);
    bool ContextMenuHeader(StringView label, ImGuiTreeNodeFlags flags = 0);
    bool ContextMenuHeader(StringView label, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags = 0);

    bool DrawStringInput(StringView id, String& value, const ImVec2& size, ImGuiInputTextFlags flags);
    void DrawMultilineText(StringView text, ImVec2 bounds, bool showEllipsis);

    bool DrawRenameInput(StringView id, String& outName, const ImVec2& size, bool& isSelected);

    void DrawInfoIcon(ImDrawList* draw_list, ImVec2 pos, onyxF32 radius, onyxU32 color);
    void DrawSearchIcon(ImDrawList* draw_list, ImVec2 offset, onyxF32 radius, onyxU32 color);
    bool DrawCloseButton(ImDrawList* draw_list, ImVec2 pos, onyxF32 size, onyxU32 color);
    void DrawFolderIcon(ImDrawList* draw_list, ImVec2 offset, onyxF32 size, onyxF32 rounding, onyxU32 color_folder, onyxU32 color_folder_lid);
}
#endif