#pragma once

#if ONYX_IS_EDITOR

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace Onyx::Ui
{
    class VectorControl
    {
    private:
        static constexpr onyxU32 BACKGROUND_COLOR_X_COMPONENT = 0xFF2619CC;
        static constexpr onyxU32 BACKGROUND_COLOR_Y_COMPONENT = 0xFF33B233;
        static constexpr onyxU32 BACKGROUND_COLOR_Z_COMPONENT = 0xFFCC3F19;
        static constexpr onyxU32 BACKGROUND_COLOR_W_COMPONENT = 0xFF808080;

    public:
        template <typename ScalarT>
        static bool Vector2Input(Vector2<ScalarT>& vector)
        {
            PrepareVectorInput(vector, Vector2<ScalarT>::Zero());

            ImGui::BeginHorizontal("##vec2Inputs");

            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();

            bool hasModified = VectorComponentInput("X", &vector[0], dataType, 50.0f, BACKGROUND_COLOR_X_COMPONENT);
            hasModified |= VectorComponentInput("Y", &vector[1], dataType, 50.0f, BACKGROUND_COLOR_Y_COMPONENT);

            ImGui::EndHorizontal();

            return hasModified;
        }


        template <typename ScalarT>
        static bool Vector3Input(Vector3<ScalarT>& outVector)
        {
            constexpr Vector3<ScalarT> minValue { std::numeric_limits<ScalarT>::lowest() };
            return Vector3Input(outVector, minValue);
        }

        template <typename ScalarT>
        static bool Vector3Input(Vector3<ScalarT>& outVector, const Vector3<ScalarT>& minValue)
        {
            PrepareVectorInput(outVector, Vector3<ScalarT>::Zero());

            onyxF32 framePaddingX = ImGui::GetStyle().FramePadding.x;

            onyxF32 componentInputSize = 50.0f;// std::clamp(availableWidth / 3.0f, 50.0f, 100.0f);

            ScopedImGuiStyle style(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 0.0f));
            ImGui::BeginHorizontal("##vec3Inputs");

            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();

            //TODO: show error tooltip or error effect on UI when clamping (red flash)
            bool hasModified = VectorComponentInput("X", &outVector.X, dataType, componentInputSize, BACKGROUND_COLOR_X_COMPONENT);
            outVector.X = std::max(outVector.X, minValue.X);

            hasModified |= VectorComponentInput("Y", &outVector.Y, dataType, componentInputSize, BACKGROUND_COLOR_Y_COMPONENT);
            outVector.Y = std::max(outVector.Y, minValue.Y);

            hasModified |= VectorComponentInput("Z", &outVector.Z, dataType, componentInputSize, BACKGROUND_COLOR_Z_COMPONENT);
            outVector.Z = std::max(outVector.Z, minValue.Z);

            ImGui::Dummy(ImVec2(framePaddingX, 0));

            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool Vector4Input(Vector4<ScalarT>& vector)
        {
            PrepareVectorInput(vector, Vector4<ScalarT>::Zero());

            ImGui::BeginHorizontal("##vec4Inputs");

            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();

            bool hasModified = VectorComponentInput("X", &vector[0], dataType, 50.0f, BACKGROUND_COLOR_X_COMPONENT);
            hasModified |= VectorComponentInput("Y", &vector[1], dataType, 50.0f, BACKGROUND_COLOR_Y_COMPONENT);
            hasModified |= VectorComponentInput("Z", &vector[2], dataType, 50.0f, BACKGROUND_COLOR_Z_COMPONENT);
            hasModified |= VectorComponentInput("W", &vector[3], dataType, 50.0f, BACKGROUND_COLOR_W_COMPONENT);

            ImGui::EndHorizontal();

            return hasModified;
        }

    private:
        template <typename T>
        static bool PrepareVectorInput(T& value, T defaultValue)
        {
            if (ImGui::BeginPopupContextWindow("##ContextMenu", ImGuiPopupFlags_MouseButtonRight))
            {
                if (Ui::Button("Reset"))
                {
                    value = defaultValue;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            return true;
        }

        static bool VectorComponentInput(StringView label, void* value, ImGuiDataType dataType, onyxF32 valueColumnMinWidth, onyxU32 backgroundColorARGB);
    };
}
#endif