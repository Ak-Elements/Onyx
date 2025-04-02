#pragma once

namespace Onyx::Input
{
    enum class ConnectionType
    {
        USB,
        Wireless
    };

    enum class GameControllerButton : onyxU16
    {
        Invalid = 0,
        Button_South,
        Button_East,
        Button_West,
        Button_North,
        Button_Back,
        Button_Guide,
        Button_Start,
        Button_LeftStick,
        Button_RightStick,
        Button_LeftShoulder,
        Button_RightShoulder,
        Button_DPad_Up,
        Button_DPad_Down,
        Button_DPad_Left,
        Button_DPad_Right,

        // Other potential buttons inspired by SDL
        Button_Misc1,           /* Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button, Google Stadia capture button) */
        Button_Right_Paddle_1,   /* Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1) */
        Button_Left_Paddle_1,    /* Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3) */
        Button_Right_Paddle_2,   /* Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2) */
        Button_Left_Paddle_2,    /* Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4) */
        Button_Touchpad,        /* PS4/PS5 touchpad button */
        Button_Misc2,           /* Additional button */
        Button_Misc3,           /* Additional button */
        Button_Misc4,           /* Additional button */
        Button_Misc5,           /* Additional button */
        Button_Misc6,           /* Additional button */

        Count
    };

    enum class GameControllerAxis : onyxU8
    {
        Invalid,
        LeftStick_AxisX,
        LeftStick_AxisY,
        RightStick_AxisX,
        RightStick_AxisY,
        LeftTrigger_Axis,
        RightTrigger_Axis,

        Count
    };

    struct GameController
    {
        onyxU32 Index = 0;
        InplaceArray<bool, static_cast<onyxU8>(GameControllerButton::Count)> ButtonStates { false };
    };
}