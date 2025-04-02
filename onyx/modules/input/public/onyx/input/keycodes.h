#pragma once

namespace Onyx::Input
{
    /**
     *  \brief Keyboard scancode representation.
     *
     *  Values of this type are used to represent keyboard keys in input events
     *
     *  The values in this enumeration are based on the USB usage page standard:
     *  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
     *
     *  and are got inspired by SDL2 which references this source too.
     *
     *  https://github.com/libsdl-org/SDL/blob/main/include/SDL_scancode.h
     */
    enum class Key : onyxU16
    {
        Invalid = 0,

        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        Key_1 = 30,
        Key_2 = 31,
        Key_3 = 32,
        Key_4 = 33,
        Key_5 = 34,
        Key_6 = 35,
        Key_7 = 36,
        Key_8 = 37,
        Key_9 = 38,
        Key_0 = 39,

        Enter = 40,
        Escape = 41,
        Backspace = 42,
        Tab = 43,
        Space = 44,

        Minus = 45,
        Equals = 46,
        LeftBracket = 47,
        RightBracket = 48,
        BackSlash = 49,
        NonUsHash = 50,
        Semicolon = 51,
        Apostrophe = 52,
        Grave = 53,
        Comma = 54,
        Period = 55,
        Slash = 56,
        CapsLock = 57,

        F1 = 58,
        F2 = 59,
        F3 = 60,
        F4 = 61,
        F5 = 62,
        F6 = 63,
        F7 = 64,
        F8 = 65,
        F9 = 66,
        F10 = 67,
        F11 = 68,
        F12 = 69,

        PrintScreen = 70,
        ScrollLock = 71,
        Pause = 72,
        Insert = 73,

        Home = 74,
        PageUp = 75,
        Delete = 76,
        End = 77,
        PageDown = 78,
        Right = 79,
        Left = 80,
        Down = 81,
        Up = 82,

        NumPad_Lock_Clear = 83,
        NumPad_Divide = 84,
        NumPad_Multiply = 85,
        NumPad_Minus = 86,
        NumPad_Plus = 87,
        NumPad_Enter = 88,
        NumPad_1 = 89,
        NumPad_2 = 90,
        NumPad_3 = 91,
        NumPad_4 = 92,
        NumPad_5 = 93,
        NumPad_6 = 94,
        NumPad_7 = 95,
        NumPad_8 = 96,
        NumPad_9 = 97,
        NumPad_0 = 98,
        NumPad_Period = 99,

        NonUsBackSlash = 100,
        Application = 101, // windows key for windows 95 and compose
        Power = 102,
        NumPad_Equals = 103,
        F13 = 104,
        F14 = 105,
        F15 = 106,
        F16 = 107,
        F17 = 108,
        F18 = 109,
        F19 = 110,
        F20 = 111,
        F21 = 112,
        F22 = 113,
        F23 = 114,
        F24 = 115,
        Execute = 116,
        Help = 117,
        Menu = 118,
        Select = 119,
        Stop = 120,
        Again = 121,// Redo
        Undo = 122,
        Cut = 123,
        Copy = 124,
        Paste = 125,
        Find = 126,
        Mute = 127,
        VolumeUp = 128,
        VolumeDown = 129,
        // Legacy keys
        //LockingCapslock=130,
        //LockingNumLock=131,
        //LockingScrollLock=132,
        NumPad_Comma = 133,
        NumPad_EqualSign_AS400 = 134, // special for AS_400 keyboards

        International1 = 135, // used on asian keyboards, see footnotes in UsbDoc
        International2 = 136,
        International3 = 137, // Yen
        International4 = 138,
        International5 = 139,
        International6 = 140,
        International7 = 141,
        International8 = 142,
        International9 = 143,
        Lang1 = 144, // Hangul/english toggle
        Lang2 = 145, // Hanja Conversion
        Lang3 = 146, // Katakana
        Lang4 = 147, // Hiragana
        Lang5 = 148, // Zenkaku/hankaku
        Lang6 = 149, // Reserved
        Lang7 = 150, // Reserved
        Lang8 = 151, // Reserved
        Lang9 = 152, // Reserved

        Alternate_Erase = 153, // e.g.: Erase-Eaze
        SysReq = 154,
        Cancel = 155,
        Clear = 156,
        Prior = 157,
        Return = 158,
        Separator = 159,
        Out = 160,
        Oper = 161,
        ClearAgain = 162,
        CrSel = 163,
        ExSel = 164,

        // 165 - 175 reserved

        NumPad_00 = 176,
        NumPad_000 = 177,
        ThousandsSeparator = 178,
        DecimalSeparator = 179,
        CurrencyUnit = 180,
        CurrencySubUnit = 181,
        NumPad_LeftParen = 182,
        NumPad_RightParen = 183,
        NumPad_LeftBrace = 184,
        NumPad_RightBrace = 185,
        NumPad_Tab = 186,
        NumPad_BackSpace = 187,
        NumPad_A = 188,
        NumPad_B = 189,
        NumPad_C = 190,
        NumPad_D = 191,
        NumPad_E = 192,
        NumPad_F = 193,
        NumPad_Xor = 194,
        NumPad_Power = 195,
        NumPad_Percent = 196,
        NumPad_Less = 197,
        NumPad_Greater = 198,
        NumPad_Ampersand = 199,
        NumPad_DoubleAmpersand = 200,
        NumPad_VerticalBar = 201,
        NumPad_DoubleVerticalBar = 202,
        NumPad_Colon = 203,
        NumPad_Hash = 204,
        NumPad_Space = 205,
        NumPad_AtSign = 206,
        NumPad_Exclamation = 207,
        NumPad_MemoryStore = 208,
        NumPad_MemoryRecall = 209,
        NumPad_MemoryClear = 210,
        NumPad_MemoryAdd = 211,
        NumPad_MemorySubtract = 212,
        NumPad_MemoryMultiply = 213,
        NumPad_MemoryDivide = 214,
        NumPad_PlusMinus = 215,
        NumPad_Clear = 216,
        NumPad_ClearEntry = 217,
        NumPad_Binary = 218,
        NumPad_Octal = 219,
        NumPad_Decimal = 220,
        NumPad_Hexadecimal = 221,

        Left_Ctrl = 224,
        Left_Shift = 225,
        Left_Alt = 226,
        Left_System = 227, // windows / mac command key
        Right_Ctrl = 228,
        Right_Shift = 229,
        Right_Alt = 230,
        Right_System = 231, // windows / mac command key

        Mode = 257,

        Audio_Next = 258,
        Audio_Previous = 259,
        Audio_Stop = 260,
        Audio_Play = 261,
        Audio_Mute = 262,
        Media_Select = 263,
        WWW = 264,
        Mail = 265,
        Calculator = 266,
        Computer = 267,
        AppControl_Search = 268,
        AppControl_Home = 269,
        AppControl_Back = 270,
        AppControl_Forward = 271,
        AppControl_Stop = 272,
        AppControl_Refresh = 273,
        AppControl_Bookmarks = 274,

        Application1 = 283,
        Application2 = 284,

        // additional media keys
        Audio_Rewind = 285,
        Audio_FastForward = 286,

        // Mobile phone keys
        Soft_Left = 287,
        Soft_Right = 288,
        Call = 289, // Accepting call
        EndCall = 290, // Ending/Rejecting call

        Count = 512
    };

    enum class Modifier : onyxU8
    {
        None = 0x00,
        Ctrl = 0x01,
        Shift = 0x02,
        Alt = 0x04,
    };
    ONYX_ENABLE_BITMASK_OPERATORS(Modifier);

    inline bool IsModifierKey(Key key)
    {
        if (key < Key::Left_Ctrl)
            return false;
        if (key > Key::Right_System)
            return false;

        return  (key == Key::Left_Ctrl) || (key == Key::Right_Ctrl) ||
            (key == Key::Left_Shift) || (key == Key::Right_Shift) ||
            (key == Key::Left_Alt) || (key == Key::Right_Alt) || 
            (key == Key::Left_System) || (key == Key::Right_System);
    }
    
}