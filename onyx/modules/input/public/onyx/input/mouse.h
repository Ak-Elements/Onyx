#pragma once

#include <onyx/input/inputid.h>

namespace Onyx::Input
{
    enum class MouseButton : onyxU16
    {
        Button_1 = 1,
        First = 1, // first is after Button_1 for the magic enum to return Button_1
        Button_2,
        Button_3,
        Button_4,
        Button_5,
        Button_6,
        Button_7,
        Button_8,

        Last,
        Invalid = InputID::Invalid
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
        Invalid = InputID::Invalid
    };

    static constexpr onyxU16 MouseAxis_Count = Enums::ToIntegral(MouseAxis::Last) - Enums::ToIntegral(MouseAxis::First);
}