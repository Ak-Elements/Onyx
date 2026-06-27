#pragma once

#include <imgui.h>
#include <imgui_internal.h>

struct ImRect;

namespace onyx::localization {
struct LocalizedString;
}

namespace onyx::ui {
enum class ButtonState { None, Hovered, Held, Pressed };

bool button( const localization::LocalizedString& localizedLabel );
bool button( const localization::LocalizedString& localizedLabel, Vector2f32 size );

bool button( StringView label );
bool button( StringView label, Vector2f32 size );

bool selectable( const localization::LocalizedString& localizedLabel, bool isSelected );
bool selectable( const localization::LocalizedString& localizedLabel, bool isSelected, bool shouldFocusOnSelect );
bool selectable( StringView label, bool isSelected );
bool selectable( StringView label, bool isSelected, bool shouldFocusOnSelect );
bool selectable( StringView label, bool isSelected, bool shouldFocusOnSelected, int32_t flags );

ButtonState buttonBehavior( int32_t id, const ImRect& boundingBox );
ButtonState buttonBehavior( int32_t id, const ImRect& boundingBox, int32_t flags );
ButtonState buttonBehavior( int32_t id, const Rect2f32& boundingBox );
ButtonState buttonBehavior( int32_t id, const Rect2f32& boundingBox, int32_t flags );

template < typename IconFn >
bool iconButton( const char* id, ImVec2 size, IconFn drawIcon ) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    ImRect bb( cursorPos, ImVec2( cursorPos.x + size.x, cursorPos.y + size.y ) );
    ImGuiID itemId = ImGui::GetID( id );

    ButtonState state = buttonBehavior( static_cast< int32_t >( itemId ), bb );

    float32 rounding = ImGui::GetStyle().FrameRounding;
    uint32_t bgColor = ( ( state == ButtonState::Held ) || ( state == ButtonState::Pressed ) )
                           ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                       : ( state == ButtonState::Hovered ) ? ImGui::GetColorU32( ImGuiCol_ButtonHovered )
                                                           : ImGui::GetColorU32( ImGuiCol_Button );

    drawList->AddRectFilled( bb.Min, bb.Max, bgColor, rounding );

    drawIcon( drawList, state );

    ImGui::ItemSize( bb );
    if( !ImGui::ItemAdd( bb, itemId ) )
        return false;

    return state == ButtonState::Pressed;
}

} // namespace onyx::ui
