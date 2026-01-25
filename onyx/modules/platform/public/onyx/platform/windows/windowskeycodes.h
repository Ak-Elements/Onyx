#pragma once

#if ONYX_IS_WINDOWS

#include <onyx/input/keycodes.h>
#include <Windows.h>

namespace Onyx::Platform::Windows
{
    static constexpr onyxU32 EXTENDED_KEYMASK = (1 << 24);
    static constexpr onyxU32 REPEATED_KEYMASK = (1 << 30);

    static constexpr Input::Key WINDOWS_KEYCODES[] =
    {
        Input::Key::Invalid,
        Input::Key::Escape,
        Input::Key::Key1,
        Input::Key::Key2,
        Input::Key::Key3,
        Input::Key::Key4,
        Input::Key::Key5,
        Input::Key::Key6,
        Input::Key::Key7,
        Input::Key::Key8,
        Input::Key::Key9,
        Input::Key::Key0,
        Input::Key::Minus,
        Input::Key::Equals,
        Input::Key::Backspace,
        Input::Key::Tab,
        Input::Key::Q,
        Input::Key::W,
        Input::Key::E,
        Input::Key::R,
        Input::Key::T,
        Input::Key::Y,
        Input::Key::U,
        Input::Key::I,
        Input::Key::O,
        Input::Key::P,
        Input::Key::LeftBracket,
        Input::Key::RightBracket,
        Input::Key::Enter,
        Input::Key::Left_Ctrl,
        Input::Key::A,
        Input::Key::S,
        Input::Key::D,
        Input::Key::F,
        Input::Key::G,
        Input::Key::H,
        Input::Key::J,
        Input::Key::K,
        Input::Key::L,
        Input::Key::Semicolon,
        Input::Key::Apostrophe,
        Input::Key::Grave,
        Input::Key::Left_Shift,
        Input::Key::BackSlash,
        Input::Key::Z,
        Input::Key::X,
        Input::Key::C,
        Input::Key::V,
        Input::Key::B,
        Input::Key::N,
        Input::Key::M,
        Input::Key::Comma,
        Input::Key::Period,
        Input::Key::Slash,
        Input::Key::Right_Shift,
        Input::Key::PrintScreen,
        Input::Key::Left_Alt,
        Input::Key::Space,
        Input::Key::CapsLock,
        Input::Key::F1,
        Input::Key::F2,
        Input::Key::F3,
        Input::Key::F4,
        Input::Key::F5,
        Input::Key::F6,
        Input::Key::F7,
        Input::Key::F8,
        Input::Key::F9,
        Input::Key::F10,
        Input::Key::NumPad_Lock_Clear,
        Input::Key::ScrollLock,
        Input::Key::Home,
        Input::Key::Up,
        Input::Key::PageUp,
        Input::Key::NumPad_Minus,
        Input::Key::Left,
        Input::Key::NumPad_5,
        Input::Key::Right,
        Input::Key::NumPad_Plus,
        Input::Key::End,
        Input::Key::Down,
        Input::Key::PageDown,
        Input::Key::Insert,
        Input::Key::Delete,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::NonUsBackSlash,
        Input::Key::F11,
        Input::Key::F12,
        Input::Key::Pause,
        Input::Key::Invalid,
        Input::Key::Left_System,
        Input::Key::Right_System,
        Input::Key::Application,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::F13,
        Input::Key::F14,
        Input::Key::F15,
        Input::Key::F16,
        Input::Key::F17,
        Input::Key::F18,
        Input::Key::F19,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::International2,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::International1,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::Invalid,
        Input::Key::International4,
        Input::Key::Invalid,
        Input::Key::International5,
        Input::Key::Invalid,
        Input::Key::International3,
        Input::Key::Invalid,
        Input::Key::Invalid
    };

    inline Input::Key VirtualKeyCodeToKey(onyxU64 virtualKeyCode)
    {
        switch (virtualKeyCode)
        {
        case VK_MODECHANGE: return Input::Key::Mode;
        case VK_SELECT: return Input::Key::Select;
        case VK_EXECUTE: return Input::Key::Execute;
        case VK_HELP: return Input::Key::Help;
        case VK_PAUSE: return Input::Key::Pause;
        case VK_NUMLOCK: return Input::Key::NumPad_Lock_Clear;

        case VK_F13: return Input::Key::F13;
        case VK_F14: return Input::Key::F14;
        case VK_F15: return Input::Key::F15;
        case VK_F16: return Input::Key::F16;
        case VK_F17: return Input::Key::F17;
        case VK_F18: return Input::Key::F18;
        case VK_F19: return Input::Key::F19;
        case VK_F20: return Input::Key::F20;
        case VK_F21: return Input::Key::F21;
        case VK_F22: return Input::Key::F22;
        case VK_F23: return Input::Key::F23;
        case VK_F24: return Input::Key::F24;

        case VK_OEM_NEC_EQUAL: return Input::Key::NumPad_Equals;
        case VK_BROWSER_BACK: return Input::Key::AppControl_Back;
        case VK_BROWSER_FORWARD: return Input::Key::AppControl_Forward;
        case VK_BROWSER_REFRESH: return Input::Key::AppControl_Refresh;
        case VK_BROWSER_STOP: return Input::Key::AppControl_Stop;
        case VK_BROWSER_SEARCH: return Input::Key::AppControl_Search;
        case VK_BROWSER_FAVORITES: return Input::Key::AppControl_Bookmarks;
        case VK_BROWSER_HOME: return Input::Key::AppControl_Home;
        case VK_VOLUME_MUTE: return Input::Key::Audio_Mute;
        case VK_VOLUME_DOWN: return Input::Key::VolumeDown;
        case VK_VOLUME_UP: return Input::Key::VolumeUp;

        case VK_MEDIA_NEXT_TRACK: return Input::Key::Audio_Next;
        case VK_MEDIA_PREV_TRACK: return Input::Key::Audio_Previous;
        case VK_MEDIA_STOP: return Input::Key::Audio_Stop;
        case VK_MEDIA_PLAY_PAUSE: return Input::Key::Audio_Play;
        case VK_LAUNCH_MAIL: return Input::Key::Mail;
        case VK_LAUNCH_MEDIA_SELECT: return Input::Key::Media_Select;

        case VK_OEM_102: return Input::Key::NonUsBackSlash;

        case VK_ATTN: return Input::Key::SysReq;
        case VK_CRSEL: return Input::Key::CrSel;
        case VK_EXSEL: return Input::Key::ExSel;
        case VK_OEM_CLEAR: return Input::Key::Clear;

        case VK_LAUNCH_APP1: return Input::Key::Application1;
        case VK_LAUNCH_APP2: return Input::Key::Application2;

        default: return Input::Key::Invalid;
        }
    }

    inline Input::Key VirtualKeyCodeToKeyFallback(onyxU64 virtualKeyCode)
    {
        switch (virtualKeyCode)
        {
            case VK_LEFT: return Input::Key::Left;
            case VK_UP: return Input::Key::Up;
            case VK_RIGHT: return Input::Key::Right;
            case VK_DOWN: return Input::Key::Down;

            default: return Input::Key::Invalid;
        }
    }

    inline bool IsKeyRepeated(onyxU64 lParam) { return (lParam & REPEATED_KEYMASK) != 0; }
    inline Input::Key ConvertWindowsKey(onyxU64 wParam, onyxU64 lParam)
    {
        onyxU64 virtualKeyCode = wParam;
        onyxU32 scanCode = (lParam >> 16) & 0xFF;
        bool isExtendedKey = (lParam & EXTENDED_KEYMASK) != 0;

        Input::Key code = VirtualKeyCodeToKey(virtualKeyCode);

        if (code == Input::Key::Invalid && scanCode <= 127)
        {
            code = WINDOWS_KEYCODES[scanCode];

            if (isExtendedKey)
            {
                switch (code)
                {
                case Input::Key::Enter:
                    code = Input::Key::NumPad_Enter;
                    break;
                case Input::Key::Left_Alt:
                    code = Input::Key::Right_Alt;
                    break;
                case Input::Key::Left_Ctrl:
                    code = Input::Key::Right_Ctrl;
                    break;
                case Input::Key::Slash:
                    code = Input::Key::NumPad_Divide;
                    break;
                case Input::Key::CapsLock:
                    code = Input::Key::NumPad_Plus;
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (code)
                {
                case Input::Key::Home:
                    code = Input::Key::NumPad_7;
                    break;
                case Input::Key::Up:
                    code = Input::Key::NumPad_8;
                    break;
                case Input::Key::PageUp:
                    code = Input::Key::NumPad_9;
                    break;
                case Input::Key::Left:
                    code = Input::Key::NumPad_4;
                    break;
                case Input::Key::Right:
                    code = Input::Key::NumPad_6;
                    break;
                case Input::Key::End:
                    code = Input::Key::NumPad_1;
                    break;
                case Input::Key::Down:
                    code = Input::Key::NumPad_2;
                    break;
                case Input::Key::PageDown:
                    code = Input::Key::NumPad_3;
                    break;
                case Input::Key::Insert:
                    code = Input::Key::NumPad_0;
                    break;
                case Input::Key::Delete:
                    code = Input::Key::NumPad_Period;
                    break;
                case Input::Key::PrintScreen:
                    code = Input::Key::NumPad_Multiply;
                    break;
                default:
                    break;
                }
            }
        }

        // on-screen keyboard fallback
        if (code == Input::Key::Invalid)
            code = VirtualKeyCodeToKeyFallback(wParam);

        ONYX_ASSERT(code != Input::Key::Invalid);
        return code;
    }
}

#endif
