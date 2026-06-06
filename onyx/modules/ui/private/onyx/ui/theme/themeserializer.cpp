#include <onyx/ui/theme/themeserializer.h>

#include <onyx/assets/asset.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/filesystem/textdeserializer.h>
#include <onyx/stream/stringstream.h>
#include <onyx/ui/theme/theme.h>

namespace {
onyx::HashMap< onyx::String, onyx::Color > g_colors;
}

namespace onyx {

template < CompileTimeString PropertyName >
void readColor( const Deserializer& deserializer, Color& outColor ) {
    StringView value;
    std::ignore = deserializer.read< PropertyName >( value );
    auto it = g_colors.find( String( value ) );
    if( it == g_colors.end() )
        return;

    outColor = it->second;
}

template <>
struct Serialization< onyx::ui::Theme::Palette > {
    static bool serialize( Serializer& serializer, const onyx::ui::Theme::Palette& colors ) { return true; }

    static bool deserialize( const Deserializer& deserializer, onyx::ui::Theme::Palette& outColors ) {
        readColor< "text" >( deserializer, outColors.Text );
        readColor< "text_disabled" >( deserializer, outColors.TextDisabled );

        readColor< "window_background" >( deserializer, outColors.WindowBackground );
        readColor< "child_background" >( deserializer, outColors.ChildBackground );
        readColor< "popup_background" >( deserializer, outColors.PopupBackground );

        readColor< "border" >( deserializer, outColors.Border );
        readColor< "border_shadow" >( deserializer, outColors.BorderShadow );

        readColor< "frame_background" >( deserializer, outColors.FrameBackground );
        readColor< "frame_background_hovered" >( deserializer, outColors.FrameBackgroundHovered );
        readColor< "frame_background_active" >( deserializer, outColors.FrameBackgroundActive );

        readColor< "title_background" >( deserializer, outColors.TitleBackground );
        readColor< "title_background_active" >( deserializer, outColors.TitleBackgroundActive );
        readColor< "title_background_collapsed" >( deserializer, outColors.TitleBackgroundCollapsed );

        readColor< "menu_bar_background" >( deserializer, outColors.MenuBarBackground );

        readColor< "scrollbar_background" >( deserializer, outColors.ScrollbarBackground );
        readColor< "scrollbar_grab" >( deserializer, outColors.ScrollbarGrab );
        readColor< "scrollbar_grab_hovered" >( deserializer, outColors.ScrollbarGrabHovered );
        readColor< "scrollbar_grab_active" >( deserializer, outColors.ScrollbarGrabActive );

        readColor< "check_mark" >( deserializer, outColors.CheckMark );

        readColor< "slider_grab" >( deserializer, outColors.SliderGrab );
        readColor< "slider_grab_active" >( deserializer, outColors.SliderGrabActive );

        readColor< "button" >( deserializer, outColors.Button );
        readColor< "button_hovered" >( deserializer, outColors.ButtonHovered );
        readColor< "button_active" >( deserializer, outColors.ButtonActive );

        readColor< "header" >( deserializer, outColors.Header );
        readColor< "header_hovered" >( deserializer, outColors.HeaderHovered );
        readColor< "header_active" >( deserializer, outColors.HeaderActive );

        readColor< "separator" >( deserializer, outColors.Separator );
        readColor< "separator_hovered" >( deserializer, outColors.SeparatorHovered );
        readColor< "separator_active" >( deserializer, outColors.SeparatorActive );

        readColor< "resize_grip" >( deserializer, outColors.ResizeGrip );
        readColor< "resize_grip_hovered" >( deserializer, outColors.ResizeGripHovered );
        readColor< "resize_grip_active" >( deserializer, outColors.ResizeGripActive );

        readColor< "tab_hovered" >( deserializer, outColors.TabHovered );
        readColor< "tab" >( deserializer, outColors.Tab );
        readColor< "tab_selected" >( deserializer, outColors.TabSelected );
        readColor< "tab_selected_overline" >( deserializer, outColors.TabSelectedOverline );
        readColor< "tab_dimmed" >( deserializer, outColors.TabDimmed );
        readColor< "tab_dimmed_selected" >( deserializer, outColors.TabDimmedSelected );
        readColor< "tab_dimmed_selected_overline" >( deserializer, outColors.TabDimmedSelectedOverline );

        readColor< "docking_preview" >( deserializer, outColors.DockingPreview );
        readColor< "docking_empty_background" >( deserializer, outColors.DockingEmptyBackground );

        readColor< "plot_lines" >( deserializer, outColors.PlotLines );
        readColor< "plot_lines_hovered" >( deserializer, outColors.PlotLinesHovered );
        readColor< "plot_histogram" >( deserializer, outColors.PlotHistogram );
        readColor< "plot_histogram_hovered" >( deserializer, outColors.PlotHistogramHovered );

        readColor< "table_header_background" >( deserializer, outColors.TableHeaderBackground );
        readColor< "table_border_strong" >( deserializer, outColors.TableBorderStrong );
        readColor< "table_border_light" >( deserializer, outColors.TableBorderLight );
        readColor< "table_row_background" >( deserializer, outColors.TableRowBackground );
        readColor< "table_row_background_alt" >( deserializer, outColors.TableRowBackgroundAlt );

        readColor< "text_link" >( deserializer, outColors.TextLink );
        readColor< "text_selected_background" >( deserializer, outColors.TextSelectedBackground );

        readColor< "drag_drop_target" >( deserializer, outColors.DragDropTarget );

        readColor< "nav_cursor" >( deserializer, outColors.NavCursor );
        readColor< "nav_windowing_highlight" >( deserializer, outColors.NavWindowingHighlight );
        readColor< "nav_windowing_dim_background" >( deserializer, outColors.NavWindowingDimBackground );
        readColor< "modal_window_dim_background" >( deserializer, outColors.ModalWindowDimBackground );

        readColor< "axis_x" >( deserializer, outColors.AxisX );
        readColor< "axis_y" >( deserializer, outColors.AxisY );
        readColor< "axis_z" >( deserializer, outColors.AxisZ );
        readColor< "axis_w" >( deserializer, outColors.AxisW );

        return true;
    }
};

template <>
struct Serialization< onyx::ui::Theme::Layout > {
    static bool serialize( Serializer& serializer, const onyx::ui::Theme::Layout& style ) { return true; }

    static bool deserialize( const Deserializer& deserializer, onyx::ui::Theme::Layout& outStyle ) {
        deserializer.read< "alpha" >( outStyle.Alpha );
        deserializer.read< "disabled_alpha" >( outStyle.DisabledAlpha );
        deserializer.read< "window_padding" >( outStyle.WindowPadding );
        deserializer.read< "window_rounding" >( outStyle.WindowRounding );
        deserializer.read< "window_border_size" >( outStyle.WindowBorderSize );
        deserializer.read< "window_min_size" >( outStyle.WindowMinSize );
        deserializer.read< "window_title_align" >( outStyle.WindowTitleAlign );
        deserializer.read< "child_rounding" >( outStyle.ChildRounding );
        deserializer.read< "child_border_size" >( outStyle.ChildBorderSize );
        deserializer.read< "popup_rounding" >( outStyle.PopupRounding );
        deserializer.read< "frame_padding" >( outStyle.FramePadding );
        deserializer.read< "frame_rounding" >( outStyle.FrameRounding );
        deserializer.read< "frame_border_size" >( outStyle.FrameBorderSize );
        deserializer.read< "item_spacing" >( outStyle.ItemSpacing );
        deserializer.read< "item_inner_spacing" >( outStyle.ItemInnerSpacing );
        deserializer.read< "indent_spacing" >( outStyle.IndentSpacing );
        deserializer.read< "cell_padding" >( outStyle.CellPadding );
        deserializer.read< "scrollbar_size" >( outStyle.ScrollbarSize );
        deserializer.read< "scrollbar_rounding" >( outStyle.ScrollbarRounding );
        deserializer.read< "grab_min_size" >( outStyle.GrabMinSize );
        deserializer.read< "grab_rounding" >( outStyle.GrabRounding );
        deserializer.read< "layout_align" >( outStyle.LayoutAlign );
        deserializer.read< "tab_rounding" >( outStyle.TabRounding );
        deserializer.read< "tab_border_size" >( outStyle.TabBorderSize );
        deserializer.read< "tab_bar_border_size" >( outStyle.TabBarBorderSize );
        deserializer.read< "tab_bar_overline_size" >( outStyle.TabBarOverlineSize );
        deserializer.read< "table_angled_headers_angle" >( outStyle.TableAngledHeadersAngle );
        deserializer.read< "table_angled_headers_text_align" >( outStyle.TableAngledHeadersTextAlign );
        deserializer.read< "button_text_align" >( outStyle.ButtonTextAlign );
        deserializer.read< "selectable_text_align" >( outStyle.SelectableTextAlign );
        deserializer.read< "separator_text_border_size" >( outStyle.SeparatorTextBorderSize );
        deserializer.read< "separator_text_align" >( outStyle.SeparatorTextAlign );
        deserializer.read< "separator_text_padding" >( outStyle.SeparatorTextPadding );
        deserializer.read< "docking_separator_size" >( outStyle.DockingSeparatorSize );
        return true;
    }
};
template <>
struct Serialization< onyx::ui::Theme > {
    static bool serialize( Serializer& serializer, const onyx::ui::Theme& theme ) { return true; }

    static bool deserialize( const Deserializer& deserializer, onyx::ui::Theme& outTheme ) {
        return deserializer.read< "style" >( outTheme.Style ) && deserializer.read< "colors" >( outTheme.Colors );
    }
};
} // namespace onyx

namespace onyx::ui {

bool ThemeSerializer::serialize( [[maybe_unused]] const assets::AssetHandle< assets::AssetInterface >& asset,
    [[maybe_unused]] const assets::AssetMetaData& meta,
    [[maybe_unused]] Serializer& serializer,
    [[maybe_unused]] const IEngine& engine ) const {
    return true;
}

bool ThemeSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                   const assets::AssetMetaData& meta,
                                   [[maybe_unused]] const Deserializer& deserializer,
                                   [[maybe_unused]] IEngine& engine ) const {
    g_colors.clear();
    Theme& theme = asset.as< Theme >();

    String content;
    if( file_system::OnyxFile::ReadAll( file_system::path::getFullPath( meta.Path ), content ) == false ) {
        return false;
    }

    file_system::TextDeserializer textDes( content );

    // TODO: this could be a Color struct that supports this parsing
    textDes.readForEach< "colors" >( g_colors,
                                     []( const Deserializer& deserializer, const String& key, Color& outColor ) {
                                         StringView colorStr;
                                         deserializer.read( colorStr );

                                         if( colorStr[ 0 ] == '#' ) {
                                             colorStr.remove_prefix( 1 );
                                             uint32_t color;
                                             std::errc error = toNumeric( colorStr, color, 16 );
                                             outColor = Color( color );
                                             return error == std::errc{};
                                         } else if( colorStr.contains( ' ' ) ) {
                                             const DynamicArray< String >& parts = split( colorStr, ' ' );
                                             Color color = g_colors.at( parts[ 0 ] );
                                             float32 alphaFloat = 1.0f;
                                             std::ignore = toNumeric( parts[ 1 ], alphaFloat );

                                             uint32_t alpha = std::ceil( alphaFloat * 255 );
                                             outColor = color;
                                             outColor.A = alpha;
                                             return true;
                                         } else {
                                             auto it = g_colors.find( String( colorStr ) );
                                             if( it == g_colors.end() )
                                                 return false;
                                             outColor = it->second;
                                         }

                                         return true;
                                     } );

    textDes.read< "theme" >( theme );
    return true;
}

} // namespace onyx::ui
