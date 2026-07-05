#pragma once

#if ONYX_IS_EDITOR

#include <onyx/localization/localization.h>
#include <onyx/ui/scalarinputoptions.h>
#include <onyx/ui/scopeddisable.h>
#include <onyx/ui/scopedindent.h>

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

template < typename T >
struct ComboOption {
    String Label;
    T Value;
};
} // namespace onyx::ui

#include <onyx/ui/propertygrid/propertygridinternal.h>

namespace onyx::ui::property_grid {
void setAssetSystem( assets::AssetSystem& assetSystem );

ImVec2 getPropertyValuePosition();

void beginPropertyGrid( StringView propertyGrid, int32_t splitMinX );
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
bool drawProperty( StringView propertyName, String& value, ImGuiInputTextFlags flags );

template < typename T > requires std::is_base_of_v< assets::AssetInterface, T >
bool drawProperty( StringView propertyName, assets::AssetHandle< T >& outAsset ) {
    assets::AssetId assetId = outAsset.getId();
    drawPropertyName( propertyName );

    bool hasModified = false;
    if( internal::drawPropertyValue( propertyName, assetId, static_cast< assets::AssetType >( T::TypeId.getId() ) ) ) {
        outAsset.setId( assetId );
        hasModified = true;
    }

    ImGui::EndHorizontal();

    return hasModified;
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
    bool hasModified = internal::drawPropertyValue( propertyName, value, flags, options, ImGuiInputTextFlags_None );
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
bool drawProperty( StringView propertyName,
                   Vector3< ScalarT >& outVector,
                   ScalarOptions< Vector3< ScalarT > > options ) {
    drawPropertyName( propertyName );

    // Draw Value
    ImGui::PushID( propertyName.data() );
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };

    const Vector3< ScalarT > min = options.Min.value_or(
        Vector3< ScalarT >( std::numeric_limits< ScalarT >::lowest() ) );
    bool hasModified = VectorControl::vectorInput( outVector, min );

    ImGui::PopID();
    ImGui::EndHorizontal();

    return hasModified;
}

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawProperty( StringView propertyName, Vector3< ScalarT >& outVector ) {
    return drawProperty( propertyName, outVector, ScalarOptions< Vector3< ScalarT > >{} );
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

template < typename DisplayUnitsT, units::Angle T > requires IsRatio< DisplayUnitsT >
bool drawProperty( StringView propertyName,
                   EulerAngles< T >& value,
                   ScalarOptions< Vector3< typename T::RepresentType > > options ) {
    using StorageUnitsT = typename T::PeriodType;
    using ScalarT = typename T::RepresentType;
    Vector3< ScalarT > displayValue{ quantityCast< DisplayUnitsT, StorageUnitsT >( value.X.count() ),
                                     quantityCast< DisplayUnitsT, StorageUnitsT >( value.Y.count() ),
                                     quantityCast< DisplayUnitsT, StorageUnitsT >( value.Z.count() ) };

    if( drawProperty( propertyName, displayValue, options ) ) {
        value.X = T( quantityCast< StorageUnitsT, DisplayUnitsT >( displayValue.X ) );
        value.Y = T( quantityCast< StorageUnitsT, DisplayUnitsT >( displayValue.Y ) );
        value.Z = T( quantityCast< StorageUnitsT, DisplayUnitsT >( displayValue.Z ) );
        return true;
    }

    return false;
}

template < typename DisplayUnitsT, units::Angle T > requires IsRatio< DisplayUnitsT >
bool drawProperty( StringView propertyName, EulerAngles< T >& value ) {
    return drawProperty< DisplayUnitsT >( propertyName,
                                          value,
                                          ScalarOptions< Vector3< typename T::RepresentType > >{} );
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

}; // namespace onyx::ui::property_grid

#include <onyx/ui/propertygrid/propertygridcollections.h>

#endif
