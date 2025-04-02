#include <onyx/ui/controls/vectorcontrol.h>

#if ONYX_IS_EDITOR

#include <imgui.h>
#include <imgui_internal.h>

#include <onyx/ui/imguistyle.h>

namespace Onyx::Ui
{
    namespace
    {
        constexpr onyxU32 INPUT_HOVERED_COLOR = 0xFFCCB399;
        constexpr onyxU32 INPUT_ACTIVE_COLOR = 0xFFCC884D;

        bool CustomInput(const char* label, void* value, ImGuiDataType dataType, const ImVec2& size)
        {
            ImGui::PushID(label);

            ImGuiStyle& style = ImGui::GetStyle();
            // Calculate input field size and position
            ImVec2 pos = ImGui::GetCursorPos();
            ImVec2 inputSize = ImVec2(size.x, size.y);

            // Convert screen coordinates for custom drawing
            ImVec2 screenPos = ImGui::GetCursorScreenPos();

            bool hovered = ImGui::IsMouseHoveringRect(screenPos, ImVec2(screenPos.x + inputSize.x, screenPos.y + inputSize.y));

            const onyxF32 horizontalPadding = ImGui::GetStyle().FramePadding.x;
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // Render border
            onyxU32 borderColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Border));
            onyxF32 frameRounding = style.FrameRounding;
            drawList->AddRect(ImVec2(screenPos.x, screenPos.y), ImVec2(screenPos.x + inputSize.x, screenPos.y + inputSize.y), borderColor, frameRounding, ImDrawFlags_RoundCornersRight);

            // Render background
            onyxU32 bgColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
            if (hovered)
            {
                bgColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
            }

            onyxF32 borderThickness = style.FrameBorderSize; // Adjust this as needed

            ImVec2 bgPos = ImVec2(screenPos.x, screenPos.y + borderThickness);
            ImVec2 bgSize = ImVec2(inputSize.x - 2 * borderThickness, inputSize.y - 2 * borderThickness);
            drawList->AddRectFilled(bgPos, ImVec2(bgPos.x + bgSize.x, bgPos.y + bgSize.y), bgColor, frameRounding - borderThickness, ImDrawFlags_RoundCornersRight);

            // Render the input field
            ScopedImGuiColor scopedColors
            {
                { ImGuiCol_Text, 0xFF000000 },
                { ImGuiCol_FrameBg, 0x00000000 },
                { ImGuiCol_FrameBgHovered, 0x00000000 },
                { ImGuiCol_FrameBgActive, 0x00000000 },
                { ImGuiCol_Border, 0x00000000 },
            };

            // Move the cursor to where the input should start using relative coordinates
            ImGui::SetCursorPos(ImVec2(pos.x + horizontalPadding, pos.y));
            ImGui::SetNextItemWidth(inputSize.x - 2 * horizontalPadding);

            bool changed = ImGui::InputScalar("##hiddenScalarInput", dataType, value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_AutoSelectAll);

            // Restore the cursor position after the custom input widget using relative positioning
            //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.ItemSpacing.x);

            ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset = 0.0f;

            ImGui::PopID();
            return changed;
        }
    }

    /*static*/ bool VectorControl::VectorComponentInput(StringView label, void* value, ImGuiDataType dataType, onyxF32 valueColumnMinWidth, onyxU32 backgroundColorARGB)
    {
        bool modified = false;

        const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        const ImVec2 labelSize = { lineHeight + 3.0f, lineHeight };

        auto DrawColoredLabel = [](const char* text, onyxU32 backgroundColor, const ImVec2& size, ImFont* font)
        {
            ImGui::PushFont(font);
            // Calculate the size of the text
            ImVec2 textSize = ImGui::CalcTextSize(text);

            // Calculate the position to center the text in the rectangle
            ImVec2 rectStartPos = ImGui::GetCursorPos();
            ImVec2 rectScreenStartPos = ImGui::GetCursorScreenPos();
            ImVec2 textStartPos = ImVec2(rectScreenStartPos.x + (size.x - textSize.x) * 0.5f, rectScreenStartPos.y + (size.y - textSize.y) * 0.5f);

            // Draw the rectangle
            ImGui::GetWindowDrawList()->AddRectFilled(rectScreenStartPos, ImVec2(rectScreenStartPos.x + size.x, rectScreenStartPos.y + size.y), backgroundColor, 3, ImDrawFlags_RoundCornersLeft);

            // Move the cursor to the text position
            ImGui::SetCursorScreenPos(textStartPos);

            ImGui::TextUnformatted(text);

            // Restore the cursor position to the end of the rectangle
            ImGui::SetCursorPos(ImVec2(rectStartPos.x + size.x, rectStartPos.y));
            ImGui::PopFont();
        };

        DrawColoredLabel(label.data(), backgroundColorARGB, labelSize, GImGui->Font);

        const onyxU32 bgColor = 0xFFCCCCCC;
        const onyxU32 borderColor = 0xFF000000;

        ScopedImGuiColor colors
        {
            { ImGuiCol_FrameBg, bgColor },
            { ImGuiCol_FrameBgHovered, INPUT_HOVERED_COLOR },
            { ImGuiCol_FrameBgActive, INPUT_ACTIVE_COLOR },
            { ImGuiCol_Border, borderColor },
        };

        ScopedImGuiStyle style
        {
            { ImGuiStyleVar_FrameRounding, 3.0f },
            { ImGuiStyleVar_FrameBorderSize, 1.0f },
        };

        modified = CustomInput(label.data(), value, dataType, ImVec2(valueColumnMinWidth, lineHeight));

        return modified;
    }
}
#endif