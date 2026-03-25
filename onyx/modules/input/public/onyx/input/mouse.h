#pragma once

namespace onyx::input {
enum class MouseButton : uint16_t {
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

static constexpr uint16_t MouseButton_Count = enums::toIntegral( MouseButton::Last ) -
                                              enums::toIntegral( MouseButton::First );

enum class MouseAxis : uint16_t {
    First = enums::toIntegral( MouseButton::Last ),

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

static constexpr uint16_t MouseAxis_Count = enums::toIntegral( MouseAxis::Last ) -
                                            enums::toIntegral( MouseAxis::First );

constexpr uint16_t ToIndex( MouseButton button ) {
#if ONYX_ASSERT_ENABLED
    constexpr uint16_t first = enums::toIntegral( MouseButton::First );
    constexpr uint16_t last = enums::toIntegral( MouseButton::Last );
    ONYX_ASSERT( enums::toIntegral( button ) > first );
    ONYX_ASSERT( enums::toIntegral( button ) < last );
#endif
    uint16_t index = enums::toIntegral( button ) - 1; // mouse buttons first is set to 0
    return index;
}
} // namespace onyx::input