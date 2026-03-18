#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/input/keycodes.h>
#include <linux/input-event-codes.h>

namespace onyx::platform::wayland
{
    onyx::input::Key ConvertWaylandKey(onyxU32 wl_key)
    {
        switch (wl_key)
        {
            using enum onyx::input::Key;
            
            case KEY_A: return A;
            case KEY_B: return B;
            case KEY_C: return C;
            case KEY_D: return D;
            case KEY_E: return E;
            case KEY_F: return F;
            case KEY_G: return G;
            case KEY_H: return H;
            case KEY_I: return I;
            case KEY_J: return J;
            case KEY_K: return K;
            case KEY_L: return L;
            case KEY_M: return M;
            case KEY_N: return N;
            case KEY_O: return O;
            case KEY_P: return P;
            case KEY_Q: return Q;
            case KEY_R: return R;
            case KEY_S: return S;
            case KEY_T: return T;
            case KEY_U: return U;
            case KEY_V: return V;
            case KEY_W: return W;
            case KEY_X: return X;
            case KEY_Y: return Y;
            case KEY_Z: return Z;
            case KEY_1: return Key1;
            case KEY_2: return Key2;
            case KEY_3: return Key3;
            case KEY_4: return Key4;
            case KEY_5: return Key5;
            case KEY_6: return Key6;
            case KEY_7: return Key7;
            case KEY_8: return Key8;
            case KEY_9: return Key9;
            case KEY_0: return Key0;
            case KEY_ENTER:     return Enter;
            case KEY_ESC:       return Escape;
            case KEY_BACKSPACE: return Backspace;
            case KEY_TAB:       return Tab;
            case KEY_SPACE:     return Space;
            case KEY_MINUS:        return Minus;
            case KEY_EQUAL:        return Equals;
            case KEY_LEFTBRACE:    return LeftBracket;
            case KEY_RIGHTBRACE:   return RightBracket;
            case KEY_BACKSLASH:    return BackSlash;
            case KEY_SEMICOLON:    return Semicolon;
            case KEY_APOSTROPHE:   return Apostrophe;
            case KEY_GRAVE:        return Grave;
            case KEY_COMMA:        return Comma;
            case KEY_DOT:          return Period;
            case KEY_SLASH:        return Slash;
            case KEY_CAPSLOCK: return CapsLock;
            case KEY_F1:  return F1;
            case KEY_F2:  return F2;
            case KEY_F3:  return F3;
            case KEY_F4:  return F4;
            case KEY_F5:  return F5;
            case KEY_F6:  return F6;
            case KEY_F7:  return F7;
            case KEY_F8:  return F8;
            case KEY_F9:  return F9;
            case KEY_F10: return F10;
            case KEY_F11: return F11;
            case KEY_F12: return F12;
            case KEY_INSERT:   return Insert;
            case KEY_DELETE:   return Delete;
            case KEY_HOME:     return Home;
            case KEY_END:      return End;
            case KEY_PAGEUP:   return PageUp;
            case KEY_PAGEDOWN: return PageDown;
            case KEY_RIGHT: return Right;
            case KEY_LEFT:  return Left;
            case KEY_DOWN:  return Down;
            case KEY_UP:    return Up;
            case KEY_NUMLOCK:   return NumPad_Lock_Clear;
            case KEY_KPSLASH:   return NumPad_Divide;
            case KEY_KPASTERISK:return NumPad_Multiply;
            case KEY_KPMINUS:   return NumPad_Minus;
            case KEY_KPPLUS:    return NumPad_Plus;
            case KEY_KPENTER:   return NumPad_Enter;
            case KEY_KP1: return NumPad_1;
            case KEY_KP2: return NumPad_2;
            case KEY_KP3: return NumPad_3;
            case KEY_KP4: return NumPad_4;
            case KEY_KP5: return NumPad_5;
            case KEY_KP6: return NumPad_6;
            case KEY_KP7: return NumPad_7;
            case KEY_KP8: return NumPad_8;
            case KEY_KP9: return NumPad_9;
            case KEY_KP0: return NumPad_0;
            case KEY_KPDOT: return NumPad_Period;
            case KEY_LEFTCTRL:   return Left_Ctrl;
            case KEY_LEFTSHIFT:  return Left_Shift;
            case KEY_LEFTALT:    return Left_Alt;
            case KEY_LEFTMETA:   return Left_System;
            case KEY_RIGHTCTRL:  return Right_Ctrl;
            case KEY_RIGHTSHIFT: return Right_Shift;
            case KEY_RIGHTALT:   return Right_Alt;
            case KEY_RIGHTMETA:  return Right_System;
        }

        return input::Key::Invalid;
    }
}


#endif