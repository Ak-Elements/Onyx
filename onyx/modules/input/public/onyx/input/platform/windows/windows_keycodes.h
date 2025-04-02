#pragma once

#if ONYX_IS_WINDOWS

#include <onyx/input/keycodes.h>
#include <Windows.h>

namespace Onyx::Input
{
    static constexpr onyxU32 EXTENDED_KEYMASK = (1 << 24);
    static constexpr onyxU32 REPEATED_KEYMASK = (1 << 30);

    static constexpr Key WINDOWS_KEYCODES[] =
    {
        Key::Invalid,
        Key::Escape,
        Key::Key_1,
        Key::Key_2,
        Key::Key_3,
        Key::Key_4,
        Key::Key_5,
        Key::Key_6,
        Key::Key_7,
        Key::Key_8,
        Key::Key_9,
        Key::Key_0,
        Key::Minus,
        Key::Equals,
        Key::Backspace,
        Key::Tab,
        Key::Q,
        Key::W,
        Key::E,
        Key::R,
        Key::T,
        Key::Y,
        Key::U,
        Key::I,
        Key::O,
        Key::P,
        Key::LeftBracket,
        Key::RightBracket,
        Key::Enter,
        Key::Left_Ctrl,
        Key::A,
        Key::S,
        Key::D,
        Key::F,
        Key::G,
        Key::H,
        Key::J,
        Key::K,
        Key::L,
        Key::Semicolon,
        Key::Apostrophe,
        Key::Grave,
        Key::Left_Shift,
        Key::BackSlash,
        Key::Z,
        Key::X,
        Key::C,
        Key::V,
        Key::B,
        Key::N,
        Key::M,
        Key::Comma,
        Key::Period,
        Key::Slash,
        Key::Right_Shift,
        Key::PrintScreen,
        Key::Left_Alt,
        Key::Space,
        Key::CapsLock,
        Key::F1,
        Key::F2,
        Key::F3,
        Key::F4,
        Key::F5,
        Key::F6,
        Key::F7,
        Key::F8,
        Key::F9,
        Key::F10,
        Key::NumPad_Lock_Clear,
        Key::ScrollLock,
        Key::Home,
        Key::Up,
        Key::PageUp,
        Key::NumPad_Minus,
        Key::Left,
        Key::NumPad_5,
        Key::Right,
        Key::NumPad_Plus,
        Key::End,
        Key::Down,
        Key::PageDown,
        Key::Insert,
        Key::Delete,
        Key::Invalid,
        Key::Invalid,
        Key::NonUsBackSlash,
        Key::F11,
        Key::F12,
        Key::Pause,
        Key::Invalid,
        Key::Left_System,
        Key::Right_System,
        Key::Application,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::F13,
        Key::F14,
        Key::F15,
        Key::F16,
        Key::F17,
        Key::F18,
        Key::F19,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::International2,
        Key::Invalid,
        Key::Invalid,
        Key::International1,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::Invalid,
        Key::International4,
        Key::Invalid,
        Key::International5,
        Key::Invalid,
        Key::International3,
        Key::Invalid,
        Key::Invalid
    };

    inline Key VirtualKeyCodeToKey(onyxU64 virtualKeyCode)
    {
        switch (virtualKeyCode)
        {
        case VK_MODECHANGE: return Key::Mode;
        case VK_SELECT: return Key::Select;
        case VK_EXECUTE: return Key::Execute;
        case VK_HELP: return Key::Help;
        case VK_PAUSE: return Key::Pause;
        case VK_NUMLOCK: return Key::NumPad_Lock_Clear;

        case VK_F13: return Key::F13;
        case VK_F14: return Key::F14;
        case VK_F15: return Key::F15;
        case VK_F16: return Key::F16;
        case VK_F17: return Key::F17;
        case VK_F18: return Key::F18;
        case VK_F19: return Key::F19;
        case VK_F20: return Key::F20;
        case VK_F21: return Key::F21;
        case VK_F22: return Key::F22;
        case VK_F23: return Key::F23;
        case VK_F24: return Key::F24;

        case VK_OEM_NEC_EQUAL: return Key::NumPad_Equals;
        case VK_BROWSER_BACK: return Key::AppControl_Back;
        case VK_BROWSER_FORWARD: return Key::AppControl_Forward;
        case VK_BROWSER_REFRESH: return Key::AppControl_Refresh;
        case VK_BROWSER_STOP: return Key::AppControl_Stop;
        case VK_BROWSER_SEARCH: return Key::AppControl_Search;
        case VK_BROWSER_FAVORITES: return Key::AppControl_Bookmarks;
        case VK_BROWSER_HOME: return Key::AppControl_Home;
        case VK_VOLUME_MUTE: return Key::Audio_Mute;
        case VK_VOLUME_DOWN: return Key::VolumeDown;
        case VK_VOLUME_UP: return Key::VolumeUp;

        case VK_MEDIA_NEXT_TRACK: return Key::Audio_Next;
        case VK_MEDIA_PREV_TRACK: return Key::Audio_Previous;
        case VK_MEDIA_STOP: return Key::Audio_Stop;
        case VK_MEDIA_PLAY_PAUSE: return Key::Audio_Play;
        case VK_LAUNCH_MAIL: return Key::Mail;
        case VK_LAUNCH_MEDIA_SELECT: return Key::Media_Select;

        case VK_OEM_102: return Key::NonUsBackSlash;

        case VK_ATTN: return Key::SysReq;
        case VK_CRSEL: return Key::CrSel;
        case VK_EXSEL: return Key::ExSel;
        case VK_OEM_CLEAR: return Key::Clear;

        case VK_LAUNCH_APP1: return Key::Application1;
        case VK_LAUNCH_APP2: return Key::Application2;

        default: return Key::Invalid;
        }
    }

    inline Key VirtualKeyCodeToKeyFallback(onyxU64 virtualKeyCode)
    {
        switch (virtualKeyCode)
        {
            case VK_LEFT: return Key::Left;
            case VK_UP: return Key::Up;
            case VK_RIGHT: return Key::Right;
            case VK_DOWN: return Key::Down;

            default: return Key::Invalid;
        }
    }

    inline bool IsKeyRepeated(onyxU64 lParam) { return (lParam & REPEATED_KEYMASK) != 0; }
    inline Key ConvertWindowsKey(onyxU64 wParam, onyxU64 lParam)
    {
        onyxU64 virtualKeyCode = wParam;
        onyxU32 scanCode = (lParam >> 16) & 0xFF;
        bool isExtendedKey = (lParam & EXTENDED_KEYMASK) != 0;

        Key code = VirtualKeyCodeToKey(virtualKeyCode);

        if (code == Key::Invalid && scanCode <= 127)
        {
            code = WINDOWS_KEYCODES[scanCode];

            if (isExtendedKey)
            {
                switch (code)
                {
                case Key::Enter:
                    code = Key::NumPad_Enter;
                    break;
                case Key::Left_Alt:
                    code = Key::Right_Alt;
                    break;
                case Key::Left_Ctrl:
                    code = Key::Right_Ctrl;
                    break;
                case Key::Slash:
                    code = Key::NumPad_Divide;
                    break;
                case Key::CapsLock:
                    code = Key::NumPad_Plus;
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (code)
                {
                case Key::Home:
                    code = Key::NumPad_7;
                    break;
                case Key::Up:
                    code = Key::NumPad_8;
                    break;
                case Key::PageUp:
                    code = Key::NumPad_9;
                    break;
                case Key::Left:
                    code = Key::NumPad_4;
                    break;
                case Key::Right:
                    code = Key::NumPad_6;
                    break;
                case Key::End:
                    code = Key::NumPad_1;
                    break;
                case Key::Down:
                    code = Key::NumPad_2;
                    break;
                case Key::PageDown:
                    code = Key::NumPad_3;
                    break;
                case Key::Insert:
                    code = Key::NumPad_0;
                    break;
                case Key::Delete:
                    code = Key::NumPad_Period;
                    break;
                case Key::PrintScreen:
                    code = Key::NumPad_Multiply;
                    break;
                default:
                    break;
                }
            }
        }

        // on-screen keyboard fallback
        if (code == Key::Invalid)
            code = VirtualKeyCodeToKeyFallback(wParam);

        ONYX_ASSERT(code != Key::Invalid);
        return code;
    }
}

#endif
