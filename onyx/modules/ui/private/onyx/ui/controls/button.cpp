#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/geometry/rect2.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/controls/button.h>

namespace onyx::ui {
bool Button( const localization::LocalizedString& localizedLabel ) {
    return Button( localizedLabel.Get() );
}

bool Button( const localization::LocalizedString& localizedLabel, Vector2f32 size ) {
    return Button( localizedLabel.Get(), size );
}

bool Button( StringView label ) {
    return ImGui::Button( label.data() );
}

bool Button( StringView label, Vector2f32 size ) {
    return ImGui::Button( label.data(), ImVec2( size[ 0 ], size[ 1 ] ) );
}

bool Selectable( const localization::LocalizedString& localizedLabel, bool isSelected ) {
    return Selectable( localizedLabel.Get(), isSelected, false, ImGuiSelectableFlags_None );
}

bool Selectable( const localization::LocalizedString& localizedLabel, bool isSelected, bool shouldFocusOnSelected ) {
    return Selectable( localizedLabel.Get(), isSelected, shouldFocusOnSelected, ImGuiSelectableFlags_None );
}

bool Selectable( StringView label, bool isSelected ) {
    return Selectable( label, isSelected, false, ImGuiSelectableFlags_None );
}

bool Selectable( StringView label, bool isSelected, bool shouldFocusOnSelected ) {
    return Selectable( label, isSelected, shouldFocusOnSelected, ImGuiSelectableFlags_None );
}

bool Selectable( StringView label, bool isSelected, bool shouldFocusOnSelected, int32_t flags ) {
    isSelected = ImGui::Selectable( label.data(), isSelected, flags );

    if ( isSelected && shouldFocusOnSelected ) {
        ImGui::SetItemDefaultFocus();
    }

    return isSelected;
}

ButtonState ButtonBehavior( int32_t id, const ImRect& boundingBox ) {
    ImVec2 min = boundingBox.GetBL();
    ImVec2 size = boundingBox.GetSize();
    return ButtonBehavior( id, Rect2f32{ min.x, min.y, size.x, size.y }, ImGuiButtonFlags_None );
}

ButtonState ButtonBehavior( int32_t id, const ImRect& boundingBox, int32_t flags ) {
    ImVec2 min = boundingBox.GetBL();
    ImVec2 size = boundingBox.GetSize();
    return ButtonBehavior( id, Rect2f32{ min.x, min.y, size.x, size.y }, flags );
}

ButtonState ButtonBehavior( int32_t id, const Rect2f32& boundingBox ) {
    return ButtonBehavior( id, boundingBox, ImGuiButtonFlags_None );
}

ButtonState ButtonBehavior( int32_t id, const Rect2f32& boundingBox, int32_t flags ) {
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
