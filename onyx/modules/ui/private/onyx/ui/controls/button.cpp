#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/geometry/rect2.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/controls/button.h>

namespace onyx::ui {
bool button( const localization::LocalizedString& localizedLabel ) {
    return button( localizedLabel.Get() );
}

bool button( const localization::LocalizedString& localizedLabel, Vector2f32 size ) {
    return button( localizedLabel.Get(), size );
}

bool button( StringView label ) {
    return ImGui::Button( label.data() );
}

bool button( StringView label, Vector2f32 size ) {
    return ImGui::Button( label.data(), ImVec2( size[ 0 ], size[ 1 ] ) );
}

bool selectable( const localization::LocalizedString& localizedLabel, bool isSelected ) {
    return selectable( localizedLabel.Get(), isSelected, false, ImGuiSelectableFlags_None );
}

bool selectable( const localization::LocalizedString& localizedLabel, bool isSelected, bool shouldFocusOnSelected ) {
    return selectable( localizedLabel.Get(), isSelected, shouldFocusOnSelected, ImGuiSelectableFlags_None );
}

bool selectable( StringView label, bool isSelected ) {
    return selectable( label, isSelected, false, ImGuiSelectableFlags_None );
}

bool selectable( StringView label, bool isSelected, bool shouldFocusOnSelected ) {
    return selectable( label, isSelected, shouldFocusOnSelected, ImGuiSelectableFlags_None );
}

bool selectable( StringView label, bool isSelected, bool shouldFocusOnSelected, int32_t flags ) {
    isSelected = ImGui::Selectable( label.data(), isSelected, flags );

    if( isSelected && shouldFocusOnSelected ) {
        ImGui::SetItemDefaultFocus();
    }

    return isSelected;
}

ButtonState buttonBehavior( int32_t id, const ImRect& boundingBox ) {
    ImVec2 min = boundingBox.GetBL();
    ImVec2 size = boundingBox.GetSize();
    return buttonBehavior( id, Rect2f32{ min.x, min.y, size.x, size.y }, ImGuiButtonFlags_None );
}

ButtonState buttonBehavior( int32_t id, const ImRect& boundingBox, int32_t flags ) {
    ImVec2 min = boundingBox.GetBL();
    ImVec2 size = boundingBox.GetSize();
    return buttonBehavior( id, Rect2f32{ min.x, min.y, size.x, size.y }, flags );
}

ButtonState buttonBehavior( int32_t id, const Rect2f32& boundingBox ) {
    return buttonBehavior( id, boundingBox, ImGuiButtonFlags_None );
}

ButtonState buttonBehavior( int32_t id, const Rect2f32& boundingBox, int32_t flags ) {
    bool isHeld = false;
    bool isHovered = false;

    const Vector2f32& min = boundingBox.getBottomLeft();
    const Vector2f32& max = boundingBox.getTopRight();

    ImRect imguiBounds{ min[ 0 ], min[ 1 ], max[ 0 ], max[ 1 ] };
    bool isPressed = ImGui::ButtonBehavior( imguiBounds, id, &isHovered, &isHeld, flags );

    return isPressed   ? ButtonState::Pressed
           : isHeld    ? ButtonState::Held
           : isHovered ? ButtonState::Hovered
                       : ButtonState::None;
}
} // namespace onyx::ui
