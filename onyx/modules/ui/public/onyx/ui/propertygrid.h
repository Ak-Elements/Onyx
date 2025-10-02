#pragma once

#if ONYX_IS_EDITOR

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/assets/asset.h>

#include <imgui_internal.h>

namespace Onyx::Assets
{
    class AssetSystem;
    struct AssetMetaData;
}

namespace Onyx::Ui
{
    enum class ScalarInputFlag
    {
        None,
        PowerOf2,
    };

    namespace PropertyGrid
    {
        void SetAssetSystem(Assets::AssetSystem& assetSystem);

        void BeginPropertyGrid(StringView propertyGrid, onyxF32 splitMinX);
        void EndPropertyGrid();

        void DrawPropertyName(StringView propertyName);
        void DrawPropertyName(StringView propertyName, StringView tooltip);
        void DrawPropertyValue(const InplaceFunction<void(), 64>& functor);

        bool BeginPropertyGroup(StringView propertyName);
        bool BeginCollapsiblePropertyGroup(StringView propertyName, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);
        bool BeginCollapsiblePropertyGroup(StringView propertyName, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

        void EndPropertyGroup();

        bool DrawStringProperty(StringView propertyName, String& value);
        bool DrawStringProperty(StringView propertyName, String& value, ImGuiInputTextFlags textFlags);

        bool DrawAssetSelector(StringView propertyName, Assets::AssetId& outAssetId, Assets::AssetType assetType);

        /* returns true if the value was modified */
        bool DrawBoolProperty(StringView propertyName, bool& value);

        /* returns true if the value was modified */
        bool DrawColorProperty(StringView propertyName, Vector3f32& color);
        bool DrawColorProperty(StringView propertyName, Vector4f32& inOutColor);
        bool DrawColorProperty(StringView propertyName, Vector4u8& inOutColor);

        template <typename ScalarT>
        bool DrawScalarProperty(StringView propertyName, ScalarT& value, ScalarInputFlag flags)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();
            bool hasModified = false;
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            if (flags == ScalarInputFlag::None)
            {
                hasModified = DrawScalarInput("##inoutScalar", dataType, value);
            }
            else if (flags == ScalarInputFlag::PowerOf2)
            {
                // TODO: Add powerof2 restriction
                hasModified = DrawScalarInput("##inoutScalar", dataType, value);
            }

            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        bool DrawScalarProperty(StringView propertyName, ScalarT& value)
        {
            return DrawScalarProperty(propertyName, value, ScalarInputFlag::None);
        }

        template <typename ScalarT>
        bool DrawVectorProperty(StringView propertyName, Vector2<ScalarT>& vector)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            bool hasModified = VectorControl::VectorInput(vector);
            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        bool DrawVectorProperty(StringView propertyName, StringView tooltip, Vector3<ScalarT>& outVector, const Vector3<ScalarT>& minValue)
        {
            DrawPropertyName(propertyName, tooltip);

            // Draw Value
            ImGui::PushID(propertyName.data());
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            bool hasModified = VectorControl::VectorInput(outVector, minValue);

            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        bool DrawVectorProperty(StringView propertyName, StringView tooltip, Vector3<ScalarT>& outVector)
        {
            constexpr Vector3<ScalarT> min{ std::numeric_limits<ScalarT>::lowest() };
            return DrawVectorProperty(propertyName, tooltip, outVector, min);
        }

        template <typename ScalarT>
        bool DrawVectorProperty(StringView propertyName, Vector3<ScalarT>& outVector)
        {
            return DrawVectorProperty(propertyName, "", outVector);
        }

        template <typename ScalarT>
        bool DrawVectorProperty(StringView propertyName, Vector4<ScalarT>& vector)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            bool hasModified = VectorControl::VectorInput(vector);
            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename EnumT> requires std::is_enum_v<EnumT>
        bool DrawEnumProperty(StringView propertyName, EnumT& currentValue)
        {
            DrawPropertyName(propertyName);

            ImGui::PushID(propertyName.data());

            bool isModified = false;
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            if (ImGui::BeginCombo("##combo", Enums::ToString(currentValue).data()))
            {
                constexpr auto enum_entries = magic_enum::enum_entries<EnumT>();
                for (auto&& [enumValue, name] : enum_entries)
                {
                    bool isSelected = enumValue == currentValue;
                    if (ImGui::Selectable(name.data(), isSelected))
                    {
                        currentValue = enumValue;
                        isModified = true;
                        break;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::PopID();
            ImGui::EndHorizontal();

            return isModified;
        }

        // Renders a enum property only with enum values between FromValue - ToValue
        template <typename EnumT, EnumT FromValue, EnumT ToValue> requires std::is_enum_v<EnumT>
        bool DrawEnumProperty(StringView propertyName, EnumT& currentValue)
        {
            DrawPropertyName(propertyName);

            ImGui::PushID(propertyName.data());

            bool isModified = false;
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            if (ImGui::BeginCombo("##combo", Enums::ToString(currentValue).data()))
            {
                constexpr auto enum_entries = magic_enum::enum_entries<EnumT>();
                for (auto&& [enumValue, name] : enum_entries)
                {
                    if ((enumValue < FromValue) || (enumValue > ToValue))
                    {
                        continue;
                    }

                    bool isSelected = enumValue == currentValue;
                    if (ImGui::Selectable(name.data(), isSelected))
                    {
                        currentValue = enumValue;
                        isModified = true;
                        break;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::PopID();
            ImGui::EndHorizontal();

            return isModified;
        }
    };
}
#endif
