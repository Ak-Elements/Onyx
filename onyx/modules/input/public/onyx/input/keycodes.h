#pragma once
#include <onyx/input/mouse.h>

namespace Onyx::Input
{

    enum class Key : onyxU16
    {
        First = Enums::ToIntegral(MouseAxis::Last),

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        Key1,
        Key2,
        Key3,
        Key4,
        Key5,
        Key6,
        Key7,
        Key8,
        Key9,
        Key0,

        Enter,
        Escape,
        Backspace,
        Tab,
        Space,

        Minus,
        Equals,
        LeftBracket,
        RightBracket,
        BackSlash,
        NonUsHash,
        Semicolon,
        Apostrophe,
        Grave,
        Comma,
        Period,
        Slash,
        CapsLock,

        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,

        PrintScreen,
        ScrollLock,
        Pause,
        Insert,

        Home,
        PageUp,
        Delete,
        End,
        PageDown,
        Right,
        Left,
        Down,
        Up,

        NumPad_Lock_Clear,
        NumPad_Divide,
        NumPad_Multiply,
        NumPad_Minus,
        NumPad_Plus,
        NumPad_Enter,
        NumPad_1,
        NumPad_2,
        NumPad_3,
        NumPad_4,
        NumPad_5,
        NumPad_6,
        NumPad_7,
        NumPad_8,
        NumPad_9,
        NumPad_0,
        NumPad_Period,

        NonUsBackSlash,
        Application,
        Power,
        NumPad_Equals,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        Execute,
        Help,
        Menu,
        Select,
        Stop,
        Again,
        Undo,
        Cut,
        Copy,
        Paste,
        Find,
        Mute,
        VolumeUp,
        VolumeDown,
        NumPad_Comma,
        NumPad_EqualSign_AS400,

        International1,
        International2,
        International3,
        International4,
        International5,
        International6,
        International7,
        International8,
        International9,
        Lang1,
        Lang2,
        Lang3,
        Lang4,
        Lang5,
        Lang6,
        Lang7,
        Lang8,
        Lang9,

        Alternate_Erase,
        SysReq,
        Cancel,
        Clear,
        Prior,
        Return,
        Separator,
        Out,
        Oper,
        ClearAgain,
        CrSel,
        ExSel,

        NumPad_00,
        NumPad_000,
        ThousandsSeparator,
        DecimalSeparator,
        CurrencyUnit,
        CurrencySubUnit,
        NumPad_LeftParen,
        NumPad_RightParen,
        NumPad_LeftBrace,
        NumPad_RightBrace,
        NumPad_Tab,
        NumPad_BackSpace,
        NumPad_A,
        NumPad_B,
        NumPad_C,
        NumPad_D,
        NumPad_E,
        NumPad_F,
        NumPad_Xor,
        NumPad_Power,
        NumPad_Percent,
        NumPad_Less,
        NumPad_Greater,
        NumPad_Ampersand,
        NumPad_DoubleAmpersand,
        NumPad_VerticalBar,
        NumPad_DoubleVerticalBar,
        NumPad_Colon,
        NumPad_Hash,
        NumPad_Space,
        NumPad_AtSign,
        NumPad_Exclamation,
        NumPad_MemoryStore,
        NumPad_MemoryRecall,
        NumPad_MemoryClear,
        NumPad_MemoryAdd,
        NumPad_MemorySubtract,
        NumPad_MemoryMultiply,
        NumPad_MemoryDivide,
        NumPad_PlusMinus,
        NumPad_Clear,
        NumPad_ClearEntry,
        NumPad_Binary,
        NumPad_Octal,
        NumPad_Decimal,
        NumPad_Hexadecimal,

        Left_Ctrl,
        Left_Shift,
        Left_Alt,
        Left_System,
        Right_Ctrl,
        Right_Shift,
        Right_Alt,
        Right_System,

        Mode,

        Audio_Next,
        Audio_Previous,
        Audio_Stop,
        Audio_Play,
        Audio_Mute,
        Media_Select,
        WWW,
        Mail,
        Calculator,
        Computer,
        AppControl_Search,
        AppControl_Home,
        AppControl_Back,
        AppControl_Forward,
        AppControl_Stop,
        AppControl_Refresh,
        AppControl_Bookmarks,

        Application1,
        Application2,

        Audio_Rewind,
        Audio_FastForward,

        Soft_Left,
        Soft_Right,
        Call,
        EndCall,

        Last,
        Invalid = 0
    };

    static constexpr onyxU16 Key_Count = Enums::ToIntegral(Key::Last) - Enums::ToIntegral(Key::First);

    inline bool IsModifierKey(Key key)
    {
        if (key < Key::Left_Ctrl)
            return false;
        if (key > Key::Right_System)
            return false;

        return (key == Key::Left_Ctrl) || (key == Key::Right_Ctrl) ||
            (key == Key::Left_Shift) || (key == Key::Right_Shift) ||
            (key == Key::Left_Alt) || (key == Key::Right_Alt) ||
            (key == Key::Left_System) || (key == Key::Right_System);
    }

    constexpr auto ToIndex(Key key) -> std::underlying_type_t<Key>
    {
        constexpr onyxU16 first = Enums::ToIntegral(Key::First);
#if ONYX_ASSERTS_ENABLED 
        constexpr onyxU16 last = Enums::ToIntegral(Key::Last);
        ONYX_ASSERT(Enums::ToIntegral(key) > first);
        ONYX_ASSERT(Enums::ToIntegral(key) < last);
#endif

        onyxU16 index = Enums::ToIntegral(key) - first;
        return index;
    }
    
}
