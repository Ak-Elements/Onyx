#pragma once

#if ONYX_IS_EDITOR

#include <onyx/assets/asset.h>
#include <onyx/ui/controls/assetselector.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/scalarinputoptions.h>
#include <onyx/ui/scopeddisable.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/scopedstyle.h>

#include <imgui.h>

namespace onyx::assets {
enum class AssetType : uint32_t;
struct AssetId;
} // namespace onyx::assets

namespace onyx::ui::property_grid::internal {

ImVec2 getPropertyValuePosition();
int32_t getSplitterPositionX();
void setSplitterPositionX( int32_t position );

ImGuiID beginPropertyGrid( StringView id, int32_t splitterMinX );
void endPropertyGrid();

template < typename T > requires std::is_base_of_v< assets::AssetInterface, T >
bool drawPropertyValue( StringView propertyName, assets::AssetHandle< T >& outAsset ) {
    ScopedImGuiId id( propertyName );
    return assetSelector( *g_uiContext.AssetSystem, outAsset );
}

bool drawPropertyValue( StringView id, StringView value );
bool drawPropertyValue( StringView id, StringView value, ImGuiInputTextFlags flags );
bool drawPropertyValue( StringView id, String& value );
bool drawPropertyValue( StringView id, String& value, ImGuiInputTextFlags flags );

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawPropertyValue( StringView propertyName,
                        ScalarT& value,
                        ScalarInputFlag flags,
                        ScalarOptions< ScalarT > options,
                        uint32_t inputFlags ) {
    bool hasModified = false;
    ScopedImGuiId scopedId( propertyName.data() );
    constexpr ImGuiDataType DataType = getImGuiDataType< ScalarT >();
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
                                 inputFlags | ImGuiInputTextFlags_CharsDecimal ) ) {
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
                                                 inputFlags );
            } else {
                drawScalarInput( "##inoutScalar",
                                 DataType,
                                 value,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 inputFlags | ImGuiInputTextFlags_CharsDecimal );
                hasModified = ImGui::IsItemDeactivatedAfterEdit() &&
                              ( ImGui::IsKeyPressed( ImGuiKey_Escape ) == false );
            }
        }
    } else if( flags == ScalarInputFlag::PowerOf2 ) {
        drawScalarInput( "##inoutScalar", DataType, value );
        hasModified = ImGui::IsItemDeactivatedAfterEdit() && ( ImGui::IsKeyPressed( ImGuiKey_Escape ) == false );
    }
    return hasModified;
}

} // namespace onyx::ui::property_grid::internal

#endif
