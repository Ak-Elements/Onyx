#pragma once

namespace onyx::input
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

    static constexpr onyxU16 MouseButton_Count = enums::ToIntegral(MouseButton::Last) - enums::ToIntegral(MouseButton::First);

    enum class MouseAxis : onyxU16
    {
        First = enums::ToIntegral(MouseButton::Last),

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

    static constexpr onyxU16 MouseAxis_Count = enums::ToIntegral(MouseAxis::Last) - enums::ToIntegral(MouseAxis::First);

    constexpr onyxU16 ToIndex(MouseButton button)
    {
#if ONYX_ASSERT_ENABLED
        constexpr onyxU16 first = enums::ToIntegral(MouseButton::First);
        constexpr onyxU16 last = enums::ToIntegral(MouseButton::Last);
        ONYX_ASSERT(enums::ToIntegral(button) > first);
        ONYX_ASSERT(enums::ToIntegral(button) < last);
#endif
        onyxU16 index = enums::ToIntegral(button) - 1; // mouse buttons first is set to 0
        return index;

    }
}