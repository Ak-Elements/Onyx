#pragma once

#include <onyx/assets/asset.h>

#include <imgui.h>

namespace onyx::ui {
class Theme : public assets::Asset< Theme > {
  public:
    struct Palette {
        Color Text{ 0xFFFFFFFF };
        Color TextDisabled{ 0x808080FF };

        Color WindowBackground{ 0x0D0D0DF0 };
        Color ChildBackground{ 0x00000000 };
        Color PopupBackground{ 0x141414F0 };

        Color Border{ 0x6E6E8080 };
        Color BorderShadow{ 0x00000000 };

        Color FrameBackground{ 0x7A4A298A };
        Color FrameBackgroundHovered{ 0xFA964266 };
        Color FrameBackgroundActive{ 0xFA9642AB };

        Color TitleBackground{ 0x0A0A0AFF };
        Color TitleBackgroundActive{ 0x7A4A29FF };
        Color TitleBackgroundCollapsed{ 0x00000082 };

        Color MenuBarBackground{ 0x242424FF };

        Color ScrollbarBackground{ 0x05050587 };
        Color ScrollbarGrab{ 0x4F4F4FFF };
        Color ScrollbarGrabHovered{ 0x696969FF };
        Color ScrollbarGrabActive{ 0x828282FF };

        Color CheckMark{ 0xFA9642FF };

        Color SliderGrab{ 0xFA9642FF };
        Color SliderGrabActive{ 0xCC8A75FF };

        Color Button{ 0xFA964266 };
        Color ButtonHovered{ 0xFA9642FF };
        Color ButtonActive{ 0xFA870FFF };

        Color Header{ 0xFA96424F };
        Color HeaderHovered{ 0xFA9642CC };
        Color HeaderActive{ 0xFA9642FF };

        Color Separator{ 0x6E6E8080 };
        Color SeparatorHovered{ 0xFA9642C8 };
        Color SeparatorActive{ 0xFA9642FF };

        Color ResizeGrip{ 0xFA964233 };
        Color ResizeGripHovered{ 0xFA9642AB };
        Color ResizeGripActive{ 0xFA9642FF };

        Color TabHovered{ 0xFA9642CC };
        Color Tab{ 0x94592EDC };
        Color TabSelected{ 0xFA9642FF };
        Color TabSelectedOverline{ 0xFA9642FF };
        Color TabDimmed{ 0x261A12DC };
        Color TabDimmedSelected{ 0x6B4224FF };
        Color TabDimmedSelectedOverline{ 0xFA964280 };

        Color DockingPreview{ 0xFA9642B3 };
        Color DockingEmptyBackground{ 0x141414FF };

        Color PlotLines{ 0x9C9C9CFF };
        Color PlotLinesHovered{ 0x596EFFFF };
        Color PlotHistogram{ 0x00B3E6FF };
        Color PlotHistogramHovered{ 0x0099FFFF };

        Color TableHeaderBackground{ 0x333030FF };
        Color TableBorderStrong{ 0x59594FFF };
        Color TableBorderLight{ 0x403B3BFF };
        Color TableRowBackground{ 0x00000000 };
        Color TableRowBackgroundAlt{ 0xFFFFFF0F };

        Color TextLink{ 0xFA9642FF };
        Color TextSelectedBackground{ 0xFA964259 };

        Color DragDropTarget{ 0xFFFF00E6 };

        Color NavCursor{ 0xFA9642FF };
        Color NavWindowingHighlight{ 0xFFFFFFB3 };
        Color NavWindowingDimBackground{ 0xCCCCCC33 };
        Color ModalWindowDimBackground{ 0xCCCCCC59 };

        Color AxisX{ 170, 68, 68, 255 };
        Color AxisY{ 68, 170, 68, 255 };
        Color AxisZ{ 68, 68, 170, 255 };
        Color AxisW{ 128, 128, 128, 255 };
    };

    struct Layout {
        float32 Alpha = 1.0f;
        float32 DisabledAlpha = 0.60f;

        Vector2f32 WindowPadding = { 8.0f, 8.0f };
        float32 WindowRounding = 0.0f;
        float32 WindowBorderSize = 1.0f;
        Vector2f32 WindowMinSize = { 32.0f, 32.0f };
        Vector2f32 WindowTitleAlign = { 0.0f, 0.5f };

        float32 ChildRounding = 0.0f;
        float32 ChildBorderSize = 1.0f;

        float32 PopupRounding = 0.0f;
        float32 PopupBorderSize = 1.0f;

        Vector2f32 FramePadding = { 4.0f, 3.0f };
        float32 FrameRounding = 0.0f;
        float32 FrameBorderSize = 0.0f;

        Vector2f32 ItemSpacing = { 8.0f, 4.0f };
        Vector2f32 ItemInnerSpacing = { 4.0f, 4.0f };

        float32 IndentSpacing = 21.0f;

        Vector2f32 CellPadding = { 4.0f, 2.0f };

        float32 ScrollbarSize = 14.0f;
        float32 ScrollbarRounding = 9.0f;

        float32 GrabMinSize = 10.0f;
        float32 GrabRounding = 0.0f;

        // NOTE: Not an ImGui field — your custom extension
        float32 LayoutAlign = 0.0f;

        float32 TabRounding = 4.0f;
        float32 TabBorderSize = 0.0f;
        float32 TabBarBorderSize = 1.0f;
        float32 TabBarOverlineSize = 0.0f;

        float32 TableAngledHeadersAngle = 0.0f;
        Vector2f32 TableAngledHeadersTextAlign = { 0.5f, 0.0f };

        Vector2f32 ButtonTextAlign = { 0.5f, 0.5f };
        Vector2f32 SelectableTextAlign = { 0.0f, 0.0f };

        float32 SeparatorTextBorderSize = 3.0f;
        Vector2f32 SeparatorTextAlign = { 0.0f, 0.5f };
        Vector2f32 SeparatorTextPadding = { 20.0f, 3.0f };

        float32 DockingSeparatorSize = 2.0f;

        float32 MouseOverlayWidth = 100.0f;
        float32 MouseOverlayHeight = 160.0f;
        float32 MouseOverlayBodyRounding = 50.0f;
        float32 MouseOverlayButtonRounding = 36.0f;
        float32 MouseOverlayButtonHeight = 65.0f;

        float32 MouseOverlayWheelWidth = 14.0f;
        float32 MouseOverlayWheelHeight = 28.0f;
    };

    static constexpr StringId32 TypeId{ "onyx::ui::assets::Theme" };
    static StringId32 getTypeId() { return TypeId; }

    void apply();

    Palette Colors;
    Layout Style;
};

} // namespace onyx::ui
