#pragma once

#if ONYX_IS_EDITOR

#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>
#include <onyx/ui/controls/vectorcontrol.h>

namespace onyx::assets {
class AssetInterface;
}

namespace onyx::assets {
class AssetSystem;
struct AssetMetaData;
enum class AssetType : uint32_t;
} // namespace onyx::assets

namespace onyx::ui {
enum class ScalarInputFlag : uint8_t {
    None,
    PowerOf2,
};

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
struct ScalarOptions {
    Optional< ScalarT > Min;
    Optional< ScalarT > Max;

    bool IsSlider = false;
};

template < typename T >
struct ComboOption {
    String Label;
    T Value;
};
namespace property_grid {
void setAssetSystem( assets::AssetSystem& assetSystem );

void beginPropertyGrid( StringView propertyGrid, float32 splitMinX );
void endPropertyGrid();

void drawPropertyName( StringView propertyName );
void drawPropertyValue( const InplaceFunction< void(), 64 >& functor );

bool beginPropertyGroup( StringView propertyName );
bool beginPropertyGroup( StringView propertyName, const InplaceFunction< bool() >& customHeader );

bool beginCollapsiblePropertyGroup( StringView propertyName, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None );
bool beginCollapsiblePropertyGroup( StringView propertyName,
                                    const InplaceFunction< bool() >& customHeader,
                                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None );

void endPropertyGroup();

void setNextPropertyTooltip( const String& tooltip );

bool drawButton( StringView propertyName );

bool drawProperty( StringView propertyName, StringView readOnlyValue );
bool drawProperty( StringView propertyName, String& value );
bool drawProperty( StringView propertyName, String& value, ImGuiInputTextFlags textFlags );

bool drawAssetSelector( StringView propertyName, assets::AssetId& outAssetId, assets::AssetType assetType );

template < typename T > requires std::is_base_of_v< assets::AssetInterface, T >
bool drawProperty( StringView propertyName, assets::AssetHandle< T >& outAsset ) {
    assets::AssetId assetId = outAsset.getId();
    if( drawAssetSelector( propertyName, assetId, static_cast< assets::AssetType >( T::TypeId.getId() ) ) ) {
        outAsset.setId( assetId );
        return true;
    }

    return false;
}

/* returns true if the value was modified */
bool drawProperty( StringView propertyName, bool& value );

/* returns true if the value was modified */
bool drawColorProperty( StringView propertyName, Vector3f32& color );
bool drawColorProperty( StringView propertyName, Vector4f32& inOutColor );
bool drawColorProperty( StringView propertyName, Vector4u8& inOutColor );

template < typename T > requires std::is_class_v< T >
bool drawProperty( StringView propertyName, T& ) {
    drawPropertyName( propertyName );
    ImGui::Text( "Missing property grid visualizer" );
    ImGui::EndHorizontal();
    return false;
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, ScalarT& value, ScalarInputFlag flags, ScalarOptions< ScalarT > options ) {
    drawPropertyName( propertyName );

    // Draw Value
    ImGui::PushID( propertyName.data() );
    constexpr ImGuiDataType DataType = getImGuiDataType< ScalarT >();
    bool hasModified = false;
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    if( flags == ScalarInputFlag::None ) {
        bool hasMin = options.Min.has_value();
        bool hasMax = options.Max.has_value();
        if( hasMin || hasMax ) {
            ScalarT minValue = options.Min.value_or( std::numeric_limits< ScalarT >::lowest() );
            ScalarT maxValue = options.Max.value_or( std::numeric_limits< ScalarT >::max() );

            ScalarT beforeValue = value;
            if( drawScalarInput( "##inoutScalar",
                                 DataType,
                                 value,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 ImGuiInputTextFlags_CharsDecimal ) ) {
                value = std::clamp( value, minValue, maxValue );
                hasModified = isEqual( beforeValue, value ) == false;
            }

            String tooltip;
            if( hasMin && hasMax ) {
                tooltip = format::format( "[ {} .. {} ]", minValue, maxValue );
            } else if( hasMin ) {
                tooltip = format::format( "[ {} .. ]", minValue );
            } else {
                tooltip = format::format( "[ .. {} ]", maxValue );
            }

            ImGui::SetItemTooltip( "%s", tooltip.c_str() );
        } else {
            if( options.IsSlider ) {
                hasModified = ImGui::DragScalar( "##inoutScalar",
                                                 DataType,
                                                 &value,
                                                 10,
                                                 &options.Min,
                                                 &options.Max,
                                                 nullptr,
                                                 ImGuiSliderFlags_None );
            } else {
                hasModified = drawScalarInput( "##inoutScalar",
                                               DataType,
                                               value,
                                               nullptr,
                                               nullptr,
                                               nullptr,
                                               ImGuiInputTextFlags_CharsDecimal );
            }
        }
    } else if( flags == ScalarInputFlag::PowerOf2 ) {
        // This works but the IsItemDeactivatedAfterEdit fires after losing focus which makes it hard to check if
        // the value actually changed static ScalarT tmpValue = value;
        drawScalarInput( "##inoutScalar", DataType, value );
        if( ImGui::IsItemDeactivatedAfterEdit() ) {
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

template < typename DisplayUnitsT, typename StorageUnitsT, typename ScalarT > requires IsRatio< DisplayUnitsT >
bool drawProperty( StringView propertyName,
                   Quantity< ScalarT, StorageUnitsT >& value,
                   ScalarOptions< ScalarT > options ) {
    using StorageQuanityT = Quantity< ScalarT, StorageUnitsT >;
    using DisplayQuanityT = Quantity< ScalarT, DisplayUnitsT >;

    ScalarT underlyingValue = quantityCast< DisplayUnitsT, StorageUnitsT >( value.count() );
    if( drawProperty( propertyName, underlyingValue, ScalarInputFlag::None, options ) ) {
        value = quantityCast< StorageQuanityT >( DisplayQuanityT( underlyingValue ) );
        return true;
    }

    return false;
}

template < typename DisplayUnitsT, typename StorageUnitsT, typename ScalarT > requires IsRatio< DisplayUnitsT >
bool drawProperty( StringView propertyName, Quantity< ScalarT, StorageUnitsT >& value ) {
    return drawProperty< DisplayUnitsT >( propertyName, value, ScalarOptions< ScalarT >{} );
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, ScalarT& value, ScalarOptions< ScalarT > options ) {
    return drawProperty( propertyName, value, ScalarInputFlag::None, options );
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, ScalarT& value, ScalarInputFlag flags ) {
    return drawProperty( propertyName, value, flags, ScalarOptions< ScalarT >{} );
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, ScalarT& value ) {
    return drawProperty( propertyName, value, ScalarInputFlag::None );
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, Vector2< ScalarT >& vector ) {
    drawPropertyName( propertyName );

    // Draw Value
    ImGui::PushID( propertyName.data() );
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    bool hasModified = VectorControl::vectorInput( vector );
    ImGui::PopID();
    ImGui::EndHorizontal();

    return hasModified;
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, Vector3< ScalarT >& outVector, const Vector3< ScalarT >& minValue ) {
    drawPropertyName( propertyName );

    // Draw Value
    ImGui::PushID( propertyName.data() );
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    bool hasModified = VectorControl::vectorInput( outVector, minValue );

    ImGui::PopID();
    ImGui::EndHorizontal();

    return hasModified;
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, Vector3< ScalarT >& outVector ) {
    constexpr Vector3< ScalarT > Min{ std::numeric_limits< ScalarT >::lowest() };
    return drawProperty( propertyName, outVector, Min );
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, Vector4< ScalarT >& vector ) {
    drawPropertyName( propertyName );

    // Draw Value
    ImGui::PushID( propertyName.data() );
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    bool hasModified = VectorControl::vectorInput( vector );
    ImGui::PopID();
    ImGui::EndHorizontal();

    return hasModified;
}

template < typename T, uint8_t N >
bool drawComboProperty( StringView propertyName,
                        InplaceArray< ComboOption< T >, N >& options,
                        int32_t& selectedIndex ) {
    drawPropertyName( propertyName );

    ImGui::PushID( propertyName.data() );

    bool isModified = false;
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };

    if( ImGui::BeginCombo( "##combo", options[ selectedIndex ].Label.c_str() ) ) {
        for( int32_t i = 0; i < numericCast< int32_t >( options.size() ); ++i ) {
            const bool isSelected = selectedIndex == i;
            if( ImGui::Selectable( options[ i ].Label.c_str(), isSelected ) ) {
                selectedIndex = i;
                isModified = isSelected == false;
                break;
            }

            if( isSelected )
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::PopID();
    ImGui::EndHorizontal();

    return isModified;
}

template < typename EnumT, EnumT... ExcludeEnumTs > requires std::is_enum_v< EnumT >
bool drawProperty( StringView propertyName, EnumT& currentValue ) {
    drawPropertyName( propertyName );

    ImGui::PushID( propertyName.data() );

    bool isModified = false;
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    if( ImGui::BeginCombo( "##combo", enums::toString( currentValue ).data() ) ) {
        constexpr auto EnumEntries = magic_enum::enum_entries< EnumT >();
        for( auto&& [ enumValue, name ] : EnumEntries ) {
            // Skip values passed in the variadic args
            if( ( ( enumValue == ExcludeEnumTs ) || ... ) )
                continue;

            bool isSelected = enumValue == currentValue;
            if( ImGui::Selectable( name.data(), isSelected ) ) {
                currentValue = enumValue;
                isModified = true;
                break;
            }

            if( isSelected )
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::PopID();
    ImGui::EndHorizontal();

    return isModified;
}

// Renders a enum property only with enum values between FromValue - ToValue
template < typename EnumT, EnumT FromValue, EnumT ToValue > requires std::is_enum_v< EnumT >
bool drawEnumPropertyFromTo( StringView propertyName, EnumT& currentValue ) {
    drawPropertyName( propertyName );

    ImGui::PushID( propertyName.data() );

    bool isModified = false;
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    if( ImGui::BeginCombo( "##combo", enums::toString( currentValue ).data() ) ) {
        constexpr auto EnumEntries = magic_enum::enum_entries< EnumT >();
        for( auto&& [ enumValue, name ] : EnumEntries ) {
            if( ( enumValue < FromValue ) || ( enumValue > ToValue ) ) {
                continue;
            }

            bool isSelected = enumValue == currentValue;
            if( ImGui::Selectable( name.data(), isSelected ) ) {
                currentValue = enumValue;
                isModified = true;
                break;
            }

            if( isSelected )
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::PopID();
    ImGui::EndHorizontal();

    return isModified;
}
}; // namespace property_grid
} // namespace onyx::ui
#endif
