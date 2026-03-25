#pragma once

struct ImRect;

namespace onyx::localization {
struct LocalizedString;
}

namespace onyx::ui {
enum class ButtonState { None, Hovered, Held, Pressed };

bool Button( const localization::LocalizedString& localizedLabel );
bool Button( const localization::LocalizedString& localizedLabel, Vector2f32 size );

bool Button( StringView label );
bool Button( StringView label, Vector2f32 size );

bool Selectable( const localization::LocalizedString& localizedLabel, bool isSelected );
bool Selectable( const localization::LocalizedString& localizedLabel, bool isSelected, bool shouldFocusOnSelect );
bool Selectable( StringView label, bool isSelected );
bool Selectable( StringView label, bool isSelected, bool shouldFocusOnSelect );
bool Selectable( StringView label, bool isSelected, bool shouldFocusOnSelected, int32_t flags );

ButtonState ButtonBehavior( int32_t id, const ImRect& boundingBox );
ButtonState ButtonBehavior( int32_t id, const ImRect& boundingBox, int32_t flags );
ButtonState ButtonBehavior( int32_t id, const Rect2f32& boundingBox );
ButtonState ButtonBehavior( int32_t id, const Rect2f32& boundingBox, int32_t flags );
} // namespace onyx::ui
