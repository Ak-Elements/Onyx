#pragma once

#if ONYX_IS_EDITOR

#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/assets/asset.h>

namespace Onyx::Assets
{
    class AssetSystem;
    struct AssetMetaData;
}

namespace Onyx::Ui
{
    class PropertyGrid
    {
    public:
        static void SetAssetSystem(Assets::AssetSystem& assetSystem);

        static void BeginPropertyGrid(const StringView& propertyGrid, onyxF32 splitMinX);
        static void EndPropertyGrid();

        static void DrawPropertyName(const StringView& propertyName);
        static void DrawPropertyValue(const InplaceFunction<void(), 64>& functor);

        static bool BeginPropertyGroup(const StringView& propertyName);
        static bool BeginCollapsiblePropertyGroup(const StringView& propertyName, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);
        static bool BeginCollapsiblePropertyGroup(const StringView& propertyName, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

        static void EndPropertyGroup();

        static bool DrawStringProperty(const StringView& propertyName, String& value);
        static bool DrawStringProperty(const StringView& propertyName, String& value, ImGuiInputTextFlags textFlags);

        static bool DrawAssetSelector(const StringView& propertyName, Assets::AssetId& outAssetId, Assets::AssetType assetType);

        /* returns true if the value was modified */
        static bool DrawBoolProperty(const StringView& propertyName, bool& value);

        /* returns true if the value was modified */
        static bool DrawColorProperty(const StringView& propertyName, Vector3f& color);
        static bool DrawColorProperty(const StringView& propertyName, Vector4f& inOutColor);
        static bool DrawColorProperty(const StringView& propertyName, Vector4u8& inOutColor);

        template <typename ScalarT>
        static bool DrawScalarProperty(const StringView& propertyName, ScalarT& value)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();
            bool hasModified = ImGui::InputScalar("##inoutScalar", dataType, &value);
            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool DrawVector2Property(const StringView& propertyName, Vector2<ScalarT>& vector)
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
        static bool DrawVector3Property(const StringView& propertyName, Vector3<ScalarT>& vector)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            bool hasModified = VectorControl::Vector3Input(vector);
            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT>
        static bool DrawVector4Property(const StringView& propertyName, Vector4<ScalarT>& vector)
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
        static bool DrawEnumProperty(const StringView& propertyName, EnumT& currentValue)
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
        static bool DrawEnumProperty(const StringView& propertyName, EnumT& currentValue)
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
