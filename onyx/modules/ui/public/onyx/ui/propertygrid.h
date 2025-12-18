#pragma once

#if ONYX_IS_EDITOR

#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/assets/assetid.h>

namespace Onyx::Assets
{
    class AssetSystem;
    struct AssetMetaData;
    enum class AssetType : onyxU32;
}

namespace Onyx::Ui
{
    enum class ScalarInputFlag
    {
        None,
        PowerOf2,
    };

    template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
    struct ScalarOptions
    {
        ScalarT Min = std::numeric_limits<ScalarT>::lowest();
        ScalarT Max = std::numeric_limits<ScalarT>::max();

        bool IsSlider = false;
    };

    namespace PropertyGrid
    {
        void SetAssetSystem(Assets::AssetSystem& assetSystem);

        void BeginPropertyGrid(StringView propertyGrid, onyxF32 splitMinX);
        void EndPropertyGrid();

        void DrawPropertyName(StringView propertyName);
        void DrawPropertyValue(const InplaceFunction<void(), 64>& functor);

        bool BeginPropertyGroup(StringView propertyName);
        bool BeginCollapsiblePropertyGroup(StringView propertyName, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);
        bool BeginCollapsiblePropertyGroup(StringView propertyName, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

        void EndPropertyGroup();

        void SetNextPropertyTooltip(const String& tooltip);

        
        bool DrawProperty(StringView propertyName, StringView readOnlyValue);
        bool DrawProperty(StringView propertyName, String& value);
        bool DrawProperty(StringView propertyName, String& value, ImGuiInputTextFlags textFlags);

        bool DrawAssetSelector(StringView propertyName, Assets::AssetId& outAssetId, Assets::AssetType assetType);

        /* returns true if the value was modified */
        bool DrawProperty(StringView propertyName, bool& value);

        /* returns true if the value was modified */
        bool DrawColorProperty(StringView propertyName, Vector3f32& color);
        bool DrawColorProperty(StringView propertyName, Vector4f32& inOutColor);
        bool DrawColorProperty(StringView propertyName, Vector4u8& inOutColor);

        template <typename T> requires std::is_class_v<T>
        bool DrawProperty(StringView propertyName, T&)
        {
            DrawPropertyName(propertyName);
            ImGui::Text("Missing property grid visualizer");
            ImGui::EndHorizontal();
            return false;
        }

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, ScalarT& value, ScalarInputFlag flags, ScalarOptions<ScalarT> options)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            constexpr ImGuiDataType dataType = GetImGuiDataType<ScalarT>();
            bool hasModified = false;
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            if (flags == ScalarInputFlag::None)
            {
                bool hasMin = (options.Min != std::numeric_limits<ScalarT>::lowest());
                bool hasMax = (options.Max != std::numeric_limits<ScalarT>::max());
                if (hasMin || hasMax)
                {
                    ScalarT beforeValue = value;
                    if (DrawScalarInput("##inoutScalar", dataType, value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_CharsDecimal))
                    {
                        value = std::clamp(value, options.Min, options.Max);
                        hasModified = IsEqual(beforeValue, value) == false;
                    }

                    String tooltip;
                    if (hasMin && hasMax)
                    {
                        tooltip = Format::Format("[ {} .. {} ]", options.Min, options.Max);
                    }
                    else if (hasMin)
                    {
                        tooltip = Format::Format("[ {} .. ]", options.Min);
                    }
                    else
                    {
                        tooltip = Format::Format("[ .. {} ]", options.Max);
                    }
                    ImGui::SetItemTooltip(tooltip.c_str());
                }
                else
                {
                    if (options.IsSlider)
                    {
                        hasModified = ImGui::DragScalar("##inoutScalar", dataType, &value, 10, &options.Min, &options.Max, nullptr, ImGuiSliderFlags_None);
                    }
                    else
                    {
                        hasModified = DrawScalarInput("##inoutScalar", dataType, value, nullptr, nullptr, nullptr, ImGuiInputTextFlags_CharsDecimal);
                    }
                    
                }
            }
            else if (flags == ScalarInputFlag::PowerOf2)
            {
                // This works but the IsItemDeactivatedAfterEdit fires after losing focus which makes it hard to check if the value actually changed
                //static ScalarT tmpValue = value;
                DrawScalarInput("##inoutScalar", dataType, value);
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                   // if (tmpValue != value)
                  //  {
                  //      value = tmpValue;
                        hasModified = true;
                  //  }
                }
            }

            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, ScalarT& value, ScalarOptions<ScalarT> options)
        {
            return DrawProperty(propertyName, value, ScalarInputFlag::None, options);
        }

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, ScalarT& value, ScalarInputFlag flags)
        {
            return DrawProperty(propertyName, value, flags, ScalarOptions<ScalarT>{});
        }

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT> 
        bool DrawProperty(StringView propertyName, ScalarT& value)
        {
            return DrawProperty(propertyName, value, ScalarInputFlag::None);
        }


        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, Vector2<ScalarT>& vector)
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

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, Vector3<ScalarT>& outVector, const Vector3<ScalarT>& minValue)
        {
            DrawPropertyName(propertyName);

            // Draw Value
            ImGui::PushID(propertyName.data());
            ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
            bool hasModified = VectorControl::VectorInput(outVector, minValue);

            ImGui::PopID();
            ImGui::EndHorizontal();

            return hasModified;
        }

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, Vector3<ScalarT>& outVector)
        {
            constexpr Vector3<ScalarT> min{ std::numeric_limits<ScalarT>::lowest() };
            return DrawProperty(propertyName, outVector, min);
        }

        template <typename ScalarT> requires std::is_arithmetic_v<ScalarT>
        bool DrawProperty(StringView propertyName, Vector4<ScalarT>& vector)
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

        template <typename EnumT, EnumT... ExcludeEnumTs> requires std::is_enum_v<EnumT>
        bool DrawProperty(StringView propertyName, EnumT& currentValue)
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
                    // Skip values passed in the variadic args
                    if (((enumValue == ExcludeEnumTs) || ...))
                        continue;

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
        bool DrawEnumPropertyFromTo(StringView propertyName, EnumT& currentValue)
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
