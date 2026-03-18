#pragma once

#if ONYX_IS_WINDOWS

#include <onyx/input/keycodes.h>
#include <Windows.h>

namespace onyx::platform::windows
{
    static constexpr onyxU32 EXTENDED_KEYMASK = (1 << 24);
    static constexpr onyxU32 REPEATED_KEYMASK = (1 << 30);

    static constexpr input::Key WINDOWS_KEYCODES[] =
    {
        input::Key::Invalid,
        input::Key::Escape,
        input::Key::Key1,
        input::Key::Key2,
        input::Key::Key3,
        input::Key::Key4,
        input::Key::Key5,
        input::Key::Key6,
        input::Key::Key7,
        input::Key::Key8,
        input::Key::Key9,
        input::Key::Key0,
        input::Key::Minus,
        input::Key::Equals,
        input::Key::Backspace,
        input::Key::Tab,
        input::Key::Q,
        input::Key::W,
        input::Key::E,
        input::Key::R,
        input::Key::T,
        input::Key::Y,
        input::Key::U,
        input::Key::I,
        input::Key::O,
        input::Key::P,
        input::Key::LeftBracket,
        input::Key::RightBracket,
        input::Key::Enter,
        input::Key::Left_Ctrl,
        input::Key::A,
        input::Key::S,
        input::Key::D,
        input::Key::F,
        input::Key::G,
        input::Key::H,
        input::Key::J,
        input::Key::K,
        input::Key::L,
        input::Key::Semicolon,
        input::Key::Apostrophe,
        input::Key::Grave,
        input::Key::Left_Shift,
        input::Key::BackSlash,
        input::Key::Z,
        input::Key::X,
        input::Key::C,
        input::Key::V,
        input::Key::B,
        input::Key::N,
        input::Key::M,
        input::Key::Comma,
        input::Key::Period,
        input::Key::Slash,
        input::Key::Right_Shift,
        input::Key::PrintScreen,
        input::Key::Left_Alt,
        input::Key::Space,
        input::Key::CapsLock,
        input::Key::F1,
        input::Key::F2,
        input::Key::F3,
        input::Key::F4,
        input::Key::F5,
        input::Key::F6,
        input::Key::F7,
        input::Key::F8,
        input::Key::F9,
        input::Key::F10,
        input::Key::NumPad_Lock_Clear,
        input::Key::ScrollLock,
        input::Key::Home,
        input::Key::Up,
        input::Key::PageUp,
        input::Key::NumPad_Minus,
        input::Key::Left,
        input::Key::NumPad_5,
        input::Key::Right,
        input::Key::NumPad_Plus,
        input::Key::End,
        input::Key::Down,
        input::Key::PageDown,
        input::Key::Insert,
        input::Key::Delete,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::NonUsBackSlash,
        input::Key::F11,
        input::Key::F12,
        input::Key::Pause,
        input::Key::Invalid,
        input::Key::Left_System,
        input::Key::Right_System,
        input::Key::Application,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::F13,
        input::Key::F14,
        input::Key::F15,
        input::Key::F16,
        input::Key::F17,
        input::Key::F18,
        input::Key::F19,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::International2,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::International1,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::Invalid,
        input::Key::International4,
        input::Key::Invalid,
        input::Key::International5,
        input::Key::Invalid,
        input::Key::International3,
        input::Key::Invalid,
        input::Key::Invalid
    };

    inline input::Key VirtualKeyCodeToKey(onyxU64 virtualKeyCode)
    {
        switch (virtualKeyCode)
        {
        case VK_MODECHANGE: return input::Key::Mode;
        case VK_SELECT: return input::Key::Select;
        case VK_EXECUTE: return input::Key::Execute;
        case VK_HELP: return input::Key::Help;
        case VK_PAUSE: return input::Key::Pause;
        case VK_NUMLOCK: return input::Key::NumPad_Lock_Clear;

        case VK_F13: return input::Key::F13;
        case VK_F14: return input::Key::F14;
        case VK_F15: return input::Key::F15;
        case VK_F16: return input::Key::F16;
        case VK_F17: return input::Key::F17;
        case VK_F18: return input::Key::F18;
        case VK_F19: return input::Key::F19;
        case VK_F20: return input::Key::F20;
        case VK_F21: return input::Key::F21;
        case VK_F22: return input::Key::F22;
        case VK_F23: return input::Key::F23;
        case VK_F24: return input::Key::F24;

        case VK_OEM_NEC_EQUAL: return input::Key::NumPad_Equals;
        case VK_BROWSER_BACK: return input::Key::AppControl_Back;
        case VK_BROWSER_FORWARD: return input::Key::AppControl_Forward;
        case VK_BROWSER_REFRESH: return input::Key::AppControl_Refresh;
        case VK_BROWSER_STOP: return input::Key::AppControl_Stop;
        case VK_BROWSER_SEARCH: return input::Key::AppControl_Search;
        case VK_BROWSER_FAVORITES: return input::Key::AppControl_Bookmarks;
        case VK_BROWSER_HOME: return input::Key::AppControl_Home;
        case VK_VOLUME_MUTE: return input::Key::Audio_Mute;
        case VK_VOLUME_DOWN: return input::Key::VolumeDown;
        case VK_VOLUME_UP: return input::Key::VolumeUp;

        case VK_MEDIA_NEXT_TRACK: return input::Key::Audio_Next;
        case VK_MEDIA_PREV_TRACK: return input::Key::Audio_Previous;
        case VK_MEDIA_STOP: return input::Key::Audio_Stop;
        case VK_MEDIA_PLAY_PAUSE: return input::Key::Audio_Play;
        case VK_LAUNCH_MAIL: return input::Key::Mail;
        case VK_LAUNCH_MEDIA_SELECT: return input::Key::Media_Select;

        case VK_OEM_102: return input::Key::NonUsBackSlash;

        case VK_ATTN: return input::Key::SysReq;
        case VK_CRSEL: return input::Key::CrSel;
        case VK_EXSEL: return input::Key::ExSel;
        case VK_OEM_CLEAR: return input::Key::Clear;

        case VK_LAUNCH_APP1: return input::Key::Application1;
        case VK_LAUNCH_APP2: return input::Key::Application2;

        default: return input::Key::Invalid;
        }
    }

    inline input::Key VirtualKeyCodeToKeyFallback(onyxU64 virtualKeyCode)
    {
        switch (virtualKeyCode)
        {
            case VK_LEFT: return input::Key::Left;
            case VK_UP: return input::Key::Up;
            case VK_RIGHT: return input::Key::Right;
            case VK_DOWN: return input::Key::Down;

            default: return input::Key::Invalid;
        }
    }

    inline bool IsKeyRepeated(onyxU64 lParam) { return (lParam & REPEATED_KEYMASK) != 0; }
    inline input::Key ConvertWindowsKey(onyxU64 wParam, onyxU64 lParam)
    {
        onyxU64 virtualKeyCode = wParam;
        onyxU32 scanCode = (lParam >> 16) & 0xFF;
        bool isExtendedKey = (lParam & EXTENDED_KEYMASK) != 0;

        input::Key code = VirtualKeyCodeToKey(virtualKeyCode);

        if (code == input::Key::Invalid && scanCode <= 127)
        {
            code = WINDOWS_KEYCODES[scanCode];

            if (isExtendedKey)
            {
                switch (code)
                {
                case input::Key::Enter:
                    code = input::Key::NumPad_Enter;
                    break;
                case input::Key::Left_Alt:
                    code = input::Key::Right_Alt;
                    break;
                case input::Key::Left_Ctrl:
                    code = input::Key::Right_Ctrl;
                    break;
                case input::Key::Slash:
                    code = input::Key::NumPad_Divide;
                    break;
                case input::Key::CapsLock:
                    code = input::Key::NumPad_Plus;
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (code)
                {
                case input::Key::Home:
                    code = input::Key::NumPad_7;
                    break;
                case input::Key::Up:
                    code = input::Key::NumPad_8;
                    break;
                case input::Key::PageUp:
                    code = input::Key::NumPad_9;
                    break;
                case input::Key::Left:
                    code = input::Key::NumPad_4;
                    break;
                case input::Key::Right:
                    code = input::Key::NumPad_6;
                    break;
                case input::Key::End:
                    code = input::Key::NumPad_1;
                    break;
                case input::Key::Down:
                    code = input::Key::NumPad_2;
                    break;
                case input::Key::PageDown:
                    code = input::Key::NumPad_3;
                    break;
                case input::Key::Insert:
                    code = input::Key::NumPad_0;
                    break;
                case input::Key::Delete:
                    code = input::Key::NumPad_Period;
                    break;
                case input::Key::PrintScreen:
                    code = input::Key::NumPad_Multiply;
                    break;
                default:
                    break;
                }
            }
        }

        // on-screen keyboard fallback
        if (code == input::Key::Invalid)
            code = VirtualKeyCodeToKeyFallback(wParam);

        ONYX_ASSERT(code != input::Key::Invalid);
        return code;
    }
}

#endif
