#pragma once

namespace Onyx::Input
{
    enum class MouseButton : onyxU16
    {
        First = 0, // first is after Button_1 for the magic enum to return Button_1
        Button_1,
        Button_2,
        Button_3,
        Button_4,
        Button_5,
        Button_6,
        Button_7,
        Button_8,

        Last,
        Invalid = 0
    };

    static constexpr onyxU16 MouseButton_Count = Enums::ToIntegral(MouseButton::Last) - Enums::ToIntegral(MouseButton::First);

    enum class MouseAxis : onyxU16
    {
        First = Enums::ToIntegral(MouseButton::Last),

        X,
        Y,
        DeltaX,
        DeltaY,
        Wheel,

        XY,
        DeltaXY,

        Last,
        Invalid = 0
    };

    static constexpr onyxU16 MouseAxis_Count = Enums::ToIntegral(MouseAxis::Last) - Enums::ToIntegral(MouseAxis::First);

    constexpr onyxU16 ToIndex(MouseButton button)
    {
        constexpr onyxU16 first = Enums::ToIntegral(MouseButton::First);
        constexpr onyxU16 last = Enums::ToIntegral(MouseButton::Last);
        ONYX_ASSERT(Enums::ToIntegral(button) > first);
        ONYX_ASSERT(Enums::ToIntegral(button) < last);

        onyxU16 index = Enums::ToIntegral(button) - 1; // mouse buttons first is set to 0
        return index;
    }
}