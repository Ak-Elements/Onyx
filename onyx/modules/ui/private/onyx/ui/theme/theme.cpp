#include <onyx/ui/theme/theme.h>

#include <imgui.h>

namespace onyx::ui {
void Theme::apply() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    auto toImGuiFloat4 = []( Color color ) { return ImGui::ColorConvertU32ToFloat4( color.toABGR() ); };

    // Base text & backgrounds
    colors[ ImGuiCol_Text ] = toImGuiFloat4( Colors.Text );
    colors[ ImGuiCol_TextDisabled ] = toImGuiFloat4( Colors.TextDisabled );
    colors[ ImGuiCol_WindowBg ] = toImGuiFloat4( Colors.WindowBackground );
    colors[ ImGuiCol_ChildBg ] = toImGuiFloat4( Colors.ChildBackground );
    colors[ ImGuiCol_PopupBg ] = toImGuiFloat4( Colors.PopupBackground );
    colors[ ImGuiCol_Border ] = toImGuiFloat4( Colors.Border );
    colors[ ImGuiCol_BorderShadow ] = toImGuiFloat4( Colors.BorderShadow );

    // Frames
    colors[ ImGuiCol_FrameBg ] = toImGuiFloat4( Colors.FrameBackground );
    colors[ ImGuiCol_FrameBgHovered ] = toImGuiFloat4( Colors.FrameBackgroundHovered );
    colors[ ImGuiCol_FrameBgActive ] = toImGuiFloat4( Colors.FrameBackgroundActive );

    // Titles
    colors[ ImGuiCol_TitleBg ] = toImGuiFloat4( Colors.TitleBackground );
    colors[ ImGuiCol_TitleBgActive ] = toImGuiFloat4( Colors.TitleBackgroundActive );
    colors[ ImGuiCol_TitleBgCollapsed ] = toImGuiFloat4( Colors.TitleBackgroundCollapsed );

    // Menu / scrollbar
    colors[ ImGuiCol_MenuBarBg ] = toImGuiFloat4( Colors.MenuBarBackground );
    colors[ ImGuiCol_ScrollbarBg ] = toImGuiFloat4( Colors.ScrollbarBackground );
    colors[ ImGuiCol_ScrollbarGrab ] = toImGuiFloat4( Colors.ScrollbarGrab );
    colors[ ImGuiCol_ScrollbarGrabHovered ] = toImGuiFloat4( Colors.ScrollbarGrabHovered );
    colors[ ImGuiCol_ScrollbarGrabActive ] = toImGuiFloat4( Colors.ScrollbarGrabActive );

    // Widgets
    colors[ ImGuiCol_CheckMark ] = toImGuiFloat4( Colors.CheckMark );
    colors[ ImGuiCol_SliderGrab ] = toImGuiFloat4( Colors.SliderGrab );
    colors[ ImGuiCol_SliderGrabActive ] = toImGuiFloat4( Colors.SliderGrabActive );
    colors[ ImGuiCol_Button ] = toImGuiFloat4( Colors.Button );
    colors[ ImGuiCol_ButtonHovered ] = toImGuiFloat4( Colors.ButtonHovered );
    colors[ ImGuiCol_ButtonActive ] = toImGuiFloat4( Colors.ButtonActive );

    // Headers
    colors[ ImGuiCol_Header ] = toImGuiFloat4( Colors.Header );
    colors[ ImGuiCol_HeaderHovered ] = toImGuiFloat4( Colors.HeaderHovered );
    colors[ ImGuiCol_HeaderActive ] = toImGuiFloat4( Colors.HeaderActive );

    // Separators
    colors[ ImGuiCol_Separator ] = toImGuiFloat4( Colors.Separator );
    colors[ ImGuiCol_SeparatorHovered ] = toImGuiFloat4( Colors.SeparatorHovered );
    colors[ ImGuiCol_SeparatorActive ] = toImGuiFloat4( Colors.SeparatorActive );

    // Resize grips
    colors[ ImGuiCol_ResizeGrip ] = toImGuiFloat4( Colors.ResizeGrip );
    colors[ ImGuiCol_ResizeGripHovered ] = toImGuiFloat4( Colors.ResizeGripHovered );
    colors[ ImGuiCol_ResizeGripActive ] = toImGuiFloat4( Colors.ResizeGripActive );

    // Tabs
    colors[ ImGuiCol_TabHovered ] = toImGuiFloat4( Colors.TabHovered );
    colors[ ImGuiCol_Tab ] = toImGuiFloat4( Colors.Tab );
    colors[ ImGuiCol_TabSelected ] = toImGuiFloat4( Colors.TabSelected );
    colors[ ImGuiCol_TabSelectedOverline ] = toImGuiFloat4( Colors.TabSelectedOverline );
    colors[ ImGuiCol_TabDimmed ] = toImGuiFloat4( Colors.TabDimmed );
    colors[ ImGuiCol_TabDimmedSelected ] = toImGuiFloat4( Colors.TabDimmedSelected );
    colors[ ImGuiCol_TabDimmedSelectedOverline ] = toImGuiFloat4( Colors.TabDimmedSelectedOverline );

    // Docking
    colors[ ImGuiCol_DockingPreview ] = toImGuiFloat4( Colors.DockingPreview );
    colors[ ImGuiCol_DockingEmptyBg ] = toImGuiFloat4( Colors.DockingEmptyBackground );

    // Plots
    colors[ ImGuiCol_PlotLines ] = toImGuiFloat4( Colors.PlotLines );
    colors[ ImGuiCol_PlotLinesHovered ] = toImGuiFloat4( Colors.PlotLinesHovered );
    colors[ ImGuiCol_PlotHistogram ] = toImGuiFloat4( Colors.PlotHistogram );
    colors[ ImGuiCol_PlotHistogramHovered ] = toImGuiFloat4( Colors.PlotHistogramHovered );

    // Tables
    colors[ ImGuiCol_TableHeaderBg ] = toImGuiFloat4( Colors.TableHeaderBackground );
    colors[ ImGuiCol_TableBorderStrong ] = toImGuiFloat4( Colors.TableBorderStrong );
    colors[ ImGuiCol_TableBorderLight ] = toImGuiFloat4( Colors.TableBorderLight );
    colors[ ImGuiCol_TableRowBg ] = toImGuiFloat4( Colors.TableRowBackground );
    colors[ ImGuiCol_TableRowBgAlt ] = toImGuiFloat4( Colors.TableRowBackgroundAlt );

    // Misc
    colors[ ImGuiCol_TextLink ] = toImGuiFloat4( Colors.TextLink );
    colors[ ImGuiCol_TextSelectedBg ] = toImGuiFloat4( Colors.TextSelectedBackground );
    colors[ ImGuiCol_DragDropTarget ] = toImGuiFloat4( Colors.DragDropTarget );
    colors[ ImGuiCol_NavCursor ] = toImGuiFloat4( Colors.NavCursor );
    colors[ ImGuiCol_NavWindowingHighlight ] = toImGuiFloat4( Colors.NavWindowingHighlight );
    colors[ ImGuiCol_NavWindowingDimBg ] = toImGuiFloat4( Colors.NavWindowingDimBackground );
    colors[ ImGuiCol_ModalWindowDimBg ] = toImGuiFloat4( Colors.ModalWindowDimBackground );

    // Scalars
    style.Alpha = Style.Alpha;
    style.DisabledAlpha = Style.DisabledAlpha;
    style.WindowRounding = Style.WindowRounding;
    style.WindowBorderSize = Style.WindowBorderSize;
    style.ChildRounding = Style.ChildRounding;
    style.ChildBorderSize = Style.ChildBorderSize;
    style.PopupRounding = Style.PopupRounding;
    style.PopupBorderSize = Style.PopupBorderSize;
    style.FrameRounding = Style.FrameRounding;
    style.FrameBorderSize = Style.FrameBorderSize;
    style.IndentSpacing = Style.IndentSpacing;
    style.ScrollbarSize = Style.ScrollbarSize;
    style.ScrollbarRounding = Style.ScrollbarRounding;
    style.GrabMinSize = Style.GrabMinSize;
    style.GrabRounding = Style.GrabRounding;
    style.TabRounding = Style.TabRounding;
    style.TabBorderSize = Style.TabBorderSize;
    style.TabBarBorderSize = Style.TabBarBorderSize;
    style.TabBarOverlineSize = Style.TabBarOverlineSize;
    style.TableAngledHeadersAngle = Style.TableAngledHeadersAngle;
    style.SeparatorTextBorderSize = Style.SeparatorTextBorderSize;
    style.DockingSeparatorSize = Style.DockingSeparatorSize;
    style.LayoutAlign = Style.LayoutAlign;

    // ImVec2 conversions
    style.WindowPadding = ImVec2( Style.WindowPadding.X, Style.WindowPadding.Y );
    style.WindowMinSize = ImVec2( Style.WindowMinSize.X, Style.WindowMinSize.Y );
    style.WindowTitleAlign = ImVec2( Style.WindowTitleAlign.X, Style.WindowTitleAlign.Y );
    style.FramePadding = ImVec2( Style.FramePadding.X, Style.FramePadding.Y );
    style.ItemSpacing = ImVec2( Style.ItemSpacing.X, Style.ItemSpacing.Y );
    style.ItemInnerSpacing = ImVec2( Style.ItemInnerSpacing.X, Style.ItemInnerSpacing.Y );
    style.CellPadding = ImVec2( Style.CellPadding.X, Style.CellPadding.Y );
    style.TableAngledHeadersTextAlign = ImVec2( Style.TableAngledHeadersTextAlign.X,
                                                Style.TableAngledHeadersTextAlign.Y );
    style.ButtonTextAlign = ImVec2( Style.ButtonTextAlign.X, Style.ButtonTextAlign.Y );
    style.SelectableTextAlign = ImVec2( Style.SelectableTextAlign.X, Style.SelectableTextAlign.Y );
    style.SeparatorTextAlign = ImVec2( Style.SeparatorTextAlign.X, Style.SeparatorTextAlign.Y );
    style.SeparatorTextPadding = ImVec2( Style.SeparatorTextPadding.X, Style.SeparatorTextPadding.Y );
}

} // namespace onyx::ui
