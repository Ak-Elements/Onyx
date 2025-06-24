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

    class PropertyGrid
    {
    public:
        static void SetAssetSystem(Assets::AssetSystem& assetSystem);

        static void BeginPropertyGrid(StringView propertyGrid, onyxF32 splitMinX);
        static void EndPropertyGrid();

        static void DrawPropertyName(StringView propertyName);
        static void DrawPropertyName(StringView propertyName, StringView tooltip);
        static void DrawPropertyValue(const InplaceFunction<void(), 64>& functor);

        static bool BeginPropertyGroup(StringView propertyName);
        static bool BeginCollapsiblePropertyGroup(StringView propertyName, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);
        static bool BeginCollapsiblePropertyGroup(StringView propertyName, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

        static void EndPropertyGroup();

        static bool DrawStringProperty(StringView propertyName, String& value);
        static bool DrawStringProperty(StringView propertyName, String& value, ImGuiInputTextFlags textFlags);

        static bool DrawAssetSelector(StringView propertyName, Assets::AssetId& outAssetId, Assets::AssetType assetType);

        /* returns true if the value was modified */
        static bool DrawBoolProperty(StringView propertyName, bool& value);

        /* returns true if the value was modified */
        static bool DrawColorProperty(StringView propertyName, Vector3f& color);
        static bool DrawColorProperty(StringView propertyName, Vector4f& inOutColor);
        static bool DrawColorProperty(StringView propertyName, Vector4u8& inOutColor);

        template <typename ScalarT>
        static bool DrawScalarProperty(StringView propertyName, ScalarT& value)
        {
            return DrawScalarProperty(propertyName, value, ScalarInputFlag::None);
        }

        template <typename ScalarT>
        static bool DrawScalarProperty(StringView propertyName, ScalarT& value, ScalarInputFlag flags)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();
            bool hasModified = false;
            if (flags == ScalarInputFlag::None)
            {
                hasModified = ImGui::InputScalar("##inoutScalar", dataType, &value);
            }
            else if (flags == ScalarInputFlag::PowerOf2)
            {
                // TODO: Add powerof2 restriction
                hasModified = ImGui::InputScalar("##inoutScalar", dataType, &value);
            }

            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool DrawVector2Property(StringView propertyName, Vector2<ScalarT>& vector)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            bool hasModified = VectorControl::Vector2Input(vector);
            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool DrawVector3Property(StringView propertyName, Vector3<ScalarT>& outVector)
        {
            return DrawVector3Property(propertyName, "", outVector);
        }

        template <typename ScalarT>
        static bool DrawVector3Property(StringView propertyName, StringView tooltip, Vector3<ScalarT>& outVector)
        {
            constexpr Vector3<ScalarT> min{ std::numeric_limits<ScalarT>::lowest() };
            return DrawVector3Property(propertyName, tooltip, outVector, min);
        }

        template <typename ScalarT>
        static bool DrawVector3Property(StringView propertyName, StringView tooltip, Vector3<ScalarT>& outVector, const Vector3<ScalarT>& minValue)
        {
            DrawPropertyName(propertyName, tooltip);

            // Draw Value
            ImGui::PushID(propertyName.data());
            bool hasModified = VectorControl::Vector3Input(outVector, minValue);

            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool DrawVector4Property(StringView propertyName, Vector4<ScalarT>& vector)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            bool hasModified = VectorControl::Vector4Input(vector);
            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename EnumT> requires std::is_enum_v<EnumT>
        static bool DrawEnumProperty(StringView propertyName, EnumT& currentValue)
        {
            DrawPropertyName(propertyName);

            ImGui::PushID(propertyName.data());

            bool isModified = false;
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
        static bool DrawEnumProperty(StringView propertyName, EnumT& currentValue)
        {
            DrawPropertyName(propertyName);

            ImGui::PushID(propertyName.data());

            bool isModified = false;
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

    private:
        static void DrawSplitter();

    private:
        static onyxF32 ms_SplitterMinX;
    };
}
#endif
