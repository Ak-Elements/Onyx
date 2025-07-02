#pragma once

struct ImRect;

namespace Onyx::Localization
{
    struct LocalizedString;
}

namespace Onyx::Ui
{
    enum class ButtonState
    {
        None,
        Hovered,
        Held,
        Pressed
    };

    bool Button(const Localization::LocalizedString& localizedLabel);
    bool Button(const Localization::LocalizedString& localizedLabel, Vector2f size);

    bool Button(StringView label);
    bool Button(StringView label, Vector2f size);

    bool Selectable(const Localization::LocalizedString& localizedLabel, bool isSelected);
    bool Selectable(const Localization::LocalizedString& localizedLabel, bool isSelected, bool shouldFocusOnSelect);
    bool Selectable(StringView label, bool isSelected);
    bool Selectable(StringView label, bool isSelected, bool shouldFocusOnSelect);
    bool Selectable(StringView label, bool isSelected, bool shouldFocusOnSelected, onyxS32 flags);

    ButtonState ButtonBehavior(onyxS32 id, const ImRect& boundingBox);
    ButtonState ButtonBehavior(onyxS32 id, const ImRect& boundingBox, onyxS32 flags);
    ButtonState ButtonBehavior(onyxS32 id, const Rect2f32& boundingBox);
    ButtonState ButtonBehavior(onyxS32 id, const Rect2f32& boundingBox, onyxS32 flags);
}
