#pragma once

namespace Onyx::Input
{
    enum class MouseButton : onyxS8
    {
        Invalid = 0,
        Button_1 = 1,
        Button_2 = 2,
        Button_3 = 3,
        Button_4 = 4,
        Button_5 = 5,
        Button_6 = 6,
        Button_7 = 7,
        Button_8 = 8,
        Button_Last = Button_8,
        Count,
        Left = Button_1,
        Right = Button_2,
        Middle = Button_3,
    };

    // Better name for this?
    enum class MouseAxis : onyxS8
    {
        Invalid = 0,
        PositionX,
        PositionY,
        DeltaX,
        DeltaY,
        Wheel,
        Count
    };
}