#pragma once

#if ONYX_IS_EDITOR

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace onyx::ui
{
    class VectorControl
    {
    public:
        static constexpr onyxU32 BACKGROUND_COLOR_X_COMPONENT = 0xFF4444AA;
        static constexpr onyxU32 BACKGROUND_COLOR_Y_COMPONENT = 0xFF44AA44;
        static constexpr onyxU32 BACKGROUND_COLOR_Z_COMPONENT = 0xFFAA4444;
        static constexpr onyxU32 BACKGROUND_COLOR_W_COMPONENT = 0xFF808080;

        template <typename ScalarT>
        static bool VectorInput(Vector2<ScalarT>& vector)
        {
            ImGui::BeginHorizontal("##vec2Inputs");

            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();

            bool hasModified = VectorComponentInput("X", vector[0], dataType, 50.0f, BACKGROUND_COLOR_X_COMPONENT);
            hasModified |= VectorComponentInput("Y", vector[1], dataType, 50.0f, BACKGROUND_COLOR_Y_COMPONENT);

            ImGui::EndHorizontal();

            return hasModified;
        }


        template <typename ScalarT>
        static bool VectorInput(Vector3<ScalarT>& outVector)
        {
            constexpr Vector3<ScalarT> minValue { std::numeric_limits<ScalarT>::lowest() };
            return VectorInput(outVector, minValue);
        }

        template <typename ScalarT>
        static bool VectorInput(Vector3<ScalarT>& outVector, const Vector3<ScalarT>& minValue)
        {
            constexpr InplaceArray<StringView, 3> labels{ "X", "Y", "Z" };
            return VectorInput(outVector, minValue, labels);
        }

        template <typename ScalarT>
        static bool VectorInput(Vector3<ScalarT>& outVector, const InplaceArray<StringView, 3>& labels)
        {
            constexpr Vector3<ScalarT> minValue{ std::numeric_limits<ScalarT>::lowest() };
            return VectorInput(outVector, minValue, labels);
        }

        template <typename ScalarT>
        static bool VectorInput(Vector3<ScalarT>& outVector, const Vector3<ScalarT>& minValue, const InplaceArray<StringView, 3>& labels)
        {
            //onyxF32 framePaddingX = ImGui::GetStyle().FramePadding.x;

            onyxF32 componentInputSize = 50.0f;

            ScopedImGuiStyle style(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 0.0f));
            ImGui::BeginHorizontal("##vec3Inputs");

            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();

            //TODO: show error tooltip or error effect on UI when clamping (red flash)
            bool hasModified = VectorComponentInput(labels[0], outVector.X, dataType, componentInputSize, BACKGROUND_COLOR_X_COMPONENT);
            outVector.X = std::max(outVector.X, minValue.X);

            hasModified |= VectorComponentInput(labels[1], outVector.Y, dataType, componentInputSize, BACKGROUND_COLOR_Y_COMPONENT);
            outVector.Y = std::max(outVector.Y, minValue.Y);

            hasModified |= VectorComponentInput(labels[2], outVector.Z, dataType, componentInputSize, BACKGROUND_COLOR_Z_COMPONENT);
            outVector.Z = std::max(outVector.Z, minValue.Z);

            ImGui::EndHorizontal();

           // RenderContextMenu(outVector);

            return hasModified;
        }

        template <typename ScalarT>
        static bool VectorInput(Vector4<ScalarT>& outVector)
        {
            constexpr InplaceArray<StringView, 4> labels{ "X", "Y", "Z", "W" };
            return VectorInput(outVector, labels);
        }

        template <typename ScalarT>
        static bool VectorInput(Vector4<ScalarT>& outVector, const InplaceArray<StringView, 4>& labels)
        {
            ImGui::BeginHorizontal("##vec4Inputs");

            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();

            bool hasModified = VectorComponentInput(labels[0], outVector[0], dataType, 50.0f, BACKGROUND_COLOR_X_COMPONENT);
            hasModified |= VectorComponentInput(labels[1], outVector[1], dataType, 50.0f, BACKGROUND_COLOR_Y_COMPONENT);
            hasModified |= VectorComponentInput(labels[2], outVector[2], dataType, 50.0f, BACKGROUND_COLOR_Z_COMPONENT);
            hasModified |= VectorComponentInput(labels[3], outVector[3], dataType, 50.0f, BACKGROUND_COLOR_W_COMPONENT);

            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool VectorComponentInput(StringView label, ScalarT& value, ImGuiDataType dataType, onyxF32 valueColumnMinWidth, onyxU32 backgroundColorARGB)
        {
            //constexpr onyxU32 INPUT_HOVERED_COLOR = 0xFFCCB399;
            //constexpr onyxU32 INPUT_ACTIVE_COLOR = 0xFFCC884D;
            ScopedImGuiId scopedId(label);
            constexpr StringView inputIdString = "##hiddenInput";
            const ImGuiID inputId = ImGui::GetID(inputIdString.data());

            bool isHovered = inputId == ImGui::GetHoveredID();
            bool isFocused = inputId == ImGui::GetActiveID();
            bool modified = false;

            {
                const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                const ImVec2 labelSize = { lineHeight + 3.0f, lineHeight };

                ScopedImGuiStyle styleOverride
                {
                    { ImGuiStyleVar_FrameRounding, 3.0f },
                    { ImGuiStyleVar_FrameBorderSize, 0.0f },
                };

                auto DrawColoredLabel = [&](const char* text, onyxU32 backgroundColor, const ImVec2& size, ImFont* font)
                    {
                        ImGui::PushFont(font);
                        // Calculate the size of the text
                        ImVec2 textSize = ImGui::CalcTextSize(text);

                        // Calculate the position to center the text in the rectangle
                        //ImVec2 rectStartPos = ImGui::GetCursorPos();
                        ImVec2 rectScreenStartPos = ImGui::GetCursorScreenPos();
                        ImVec2 rectScreenEndPos = ImVec2(rectScreenStartPos.x + size.x, rectScreenStartPos.y + size.y);

                        ImGui::ItemAdd(ImRect(rectScreenStartPos, rectScreenEndPos), ImGui::GetID("##labelBg"));
                        ImGui::GetWindowDrawList()->AddRectFilled(rectScreenStartPos, rectScreenEndPos, backgroundColor, 3, ImDrawFlags_RoundCornersLeft);

                        ImVec2 textStartPos = ImVec2(rectScreenStartPos.x + (size.x - textSize.x) * 0.5f, rectScreenStartPos.y + (size.y - textSize.y) * 0.5f);

                        ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset = 0.0f;
                        ImGui::SetCursorScreenPos(textStartPos);

                        ImGui::TextUnformatted(text);

                        // Restore the cursor position to the end of the rectangle
                        ImGui::SetCursorScreenPos(ImVec2(rectScreenStartPos.x + size.x, rectScreenStartPos.y));
                        ImGui::PopFont();
                    };

                ImGui::BeginGroup();
                DrawColoredLabel(label.data(), backgroundColorARGB, labelSize, GImGui->Font);
                modified = CustomInput(inputId, inputIdString, value, dataType, ImVec2(valueColumnMinWidth, lineHeight));
                ImGui::EndGroup();
            }

            ImGuiStyle& style = ImGui::GetStyle();
            if (style.FrameBorderSize > 0.0f)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                onyxU32 borderColor = isHovered ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) : isFocused ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : 0x00;
                drawList->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), borderColor, style.FrameRounding);
            }

            return modified;
        }

    private:
        template <typename T>
        static bool RenderContextMenu(T& vector)
        {
            bool hasModified = false;
            if (ImGui::BeginPopupContextItem("##ContextMenu", ImGuiPopupFlags_MouseButtonRight))
            {
                if (ui::Button("Reset"))
                {
                    vector = T::Zero();
                    ImGui::CloseCurrentPopup();
                    hasModified = true;
                }

                ImGui::EndPopup();
            }

            return hasModified;
        }

        template <typename ScalarT>
        static bool CustomInput(ImGuiID id, StringView idString, ScalarT& value, ImGuiDataType dataType, const ImVec2& size)
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec2 inputSize = ImVec2(size.x, size.y);

            // Convert screen coordinates for custom drawing
            ImVec2 screenPos = ImGui::GetCursorScreenPos();

            bool isFocused = id == ImGui::GetActiveID();
            bool hovered = ImGui::IsMouseHoveringRect(screenPos, ImVec2(screenPos.x + inputSize.x, screenPos.y + inputSize.y));

            //const onyxF32 horizontalPadding = ImGui::GetStyle().FramePadding.x;
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // Render border
            onyxF32 frameRounding = style.FrameRounding;
            onyxF32 borderSize = style.FrameBorderSize;
            if (style.FrameBorderSize > 0.0f)
            {
                onyxU32 borderColor = hovered ? ImGui::GetColorU32(ImGuiCol_ButtonHovered) : isFocused ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_Border);
                ImVec2 endPos(screenPos.x + inputSize.x, screenPos.y + inputSize.y);
                drawList->PathRect(screenPos + ImVec2(0.50f, 0.50f), endPos - ImVec2(0.50f, 0.50f), frameRounding, ImDrawFlags_RoundCornersRight);
                drawList->PathStroke(borderColor, ImDrawFlags_None, borderSize);
            }

            // Render background
            onyxU32 bgColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
            if (hovered)
            {
                bgColor = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
            }

            ImVec2 bgPos = ImVec2(screenPos.x, screenPos.y + borderSize);
            ImVec2 bgSize = ImVec2(inputSize.x - borderSize, inputSize.y - 2 * borderSize);

            drawList->AddRectFilled(bgPos, ImVec2(bgPos.x + bgSize.x, bgPos.y + bgSize.y), bgColor, frameRounding - borderSize, ImDrawFlags_RoundCornersRight);

            // Move the cursor to where the input should start using relative coordinates
            //ImGui::SetCursorScreenPos(ImVec2(screenPos.x + horizontalPadding, screenPos.y));
            
            StringView format = "{}";
            if constexpr ( std::is_floating_point_v<ScalarT> )
            {
                if ( IsEqual(std::floor(value), value) && ( isFocused == false ))
                {
                    format = "{:.1f}";
                }
                else
                {
                    format = "{:2.6g}";
                }
            }
            
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
            ImGui::SetNextItemWidth(inputSize.x);

            // Render the input field
            ScopedImGuiColor scopedColorOverride
            {
                { ImGuiCol_FrameBg, 0x00000000 },
                { ImGuiCol_FrameBgHovered, 0x00000000 },
                { ImGuiCol_FrameBgActive, 0x00000000 },
                { ImGuiCol_Border, 0x00000000 },
            };

            bool changed = DrawScalarInput(idString.data(), dataType, value, nullptr, nullptr, format.data(), flags);
            return changed;
        }
    };
}
#endif