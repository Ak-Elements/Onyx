#include <onyx/platform/linux/wayland/waylandkeyboard.h>

#include <onyx/platform/linux/wayland/waylandinput.h>
#include <onyx/input/inputsystem.h>
#include <onyx/input/inputevent.h>

#include <wayland-client.h>
#include <linux/input-event-codes.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-keysyms.h>

#include <xkbcommon/xkbcommon-compose.h>

#include <sys/mman.h>

namespace Onyx::Platform
{
    namespace
    {
        Input::Key WaylandKeyToKey(onyxU32 wl_key)
        {
            switch (wl_key)
            {
                using enum Input::Key;
                // Letters
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

                // Numbers (top row)
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

                // Basic keys
                case KEY_ENTER:     return Enter;
                case KEY_ESC:       return Escape;
                case KEY_BACKSPACE: return Backspace;
                case KEY_TAB:       return Tab;
                case KEY_SPACE:     return Space;

                // Symbols
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

                // Function keys
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

                // Navigation
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

                // Numpad
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

                // Modifiers
                case KEY_LEFTCTRL:   return Left_Ctrl;
                case KEY_LEFTSHIFT:  return Left_Shift;
                case KEY_LEFTALT:    return Left_Alt;
                case KEY_LEFTMETA:   return Left_System;

                case KEY_RIGHTCTRL:  return Right_Ctrl;
                case KEY_RIGHTSHIFT: return Right_Shift;
                case KEY_RIGHTALT:   return Right_Alt;
                case KEY_RIGHTMETA:  return Right_System;
            }

            return Input::Key::Invalid;
        }

        xkb_keysym_t composeSymbol(Xkb& xkb, xkb_keysym_t sym)
        {
            if (sym == XKB_KEY_NoSymbol || !xkb.ComposeState)
                return sym;
            if (xkb_compose_state_feed(xkb.ComposeState, sym)
                    != XKB_COMPOSE_FEED_ACCEPTED)
                return sym;
            switch (xkb_compose_state_get_status(xkb.ComposeState))
            {
                case XKB_COMPOSE_COMPOSED:
                    return xkb_compose_state_get_one_sym(xkb.ComposeState);
                case XKB_COMPOSE_COMPOSING:
                case XKB_COMPOSE_CANCELLED:
                    return XKB_KEY_NoSymbol;
                case XKB_COMPOSE_NOTHING:
                default:
                    return sym;
            }
        }

        onyxU32 GetInputChar(Xkb& xkb, uint32_t scancode)
        {
            const xkb_keysym_t* keysyms;
            const xkb_keycode_t keycode = scancode + 8;

            if (xkb_state_key_get_syms(xkb.GetState(), keycode, &keysyms) == 1)
            {
                const xkb_keysym_t keysym = composeSymbol(xkb, keysyms[0]);
                const uint32_t codepoint = ::xkb_keysym_to_utf32(keysym);
                if (codepoint != 0)
                {
                    //const int mods = _glfw.wl.xkb.modifiers;
                    //const int plain = !(mods & (GLFW_MOD_CONTROL | GLFW_MOD_ALT));
                    //_glfwInputChar(window, codepoint, mods, plain);
                    return codepoint;
                }
            }

            return 0;
        }
    }

    WaylandKeyboard::WaylandKeyboard(WaylandInput& input, wl_keyboard* keyboard)
        : m_Input(&input)
        , m_Keyboard(keyboard)
    {
        static const struct wl_keyboard_listener keyboard_listener =
        { OnKeyMap, OnEnterSurface, OnLeaveSurface, OnKeyChange, OnModifierChange, };
        wl_keyboard_add_listener(m_Keyboard, &keyboard_listener, this);
    }

    WaylandKeyboard::~WaylandKeyboard()
    {
        if (m_Keyboard != nullptr)
        {
            wl_keyboard_destroy(m_Keyboard);
        }
    }

    /*static*/ void WaylandKeyboard::OnKeyMap(void* instance, wl_keyboard* keyboard, onyxU32 format, onyxS32 fd, onyxU32 size)
    {
        WaylandKeyboard& keyboardInstance = *reinterpret_cast<WaylandKeyboard*>(instance);
        ONYX_ASSERT(keyboardInstance.m_Input != nullptr);

        WaylandInput& waylandInput = *keyboardInstance.m_Input;
        WaylandPlatformContext& context = waylandInput.GetContext();
        Xkb& xkb = context.GetXkb();

        // inspired by glfw
        ::xkb_keymap* keymap;
        ::xkb_state* state;
        ::xkb_compose_table* composeTable;
        ::xkb_compose_state* composeState;

        char* mapStr;
        const char* locale;

        if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
        {
            ::close(fd);
            return;
        }

        mapStr = reinterpret_cast<char*>(::mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0));
        if (mapStr == MAP_FAILED)
        {
            ::close(fd);
            return;
        }

        keymap = ::xkb_keymap_new_from_string(xkb.Context,
                                        mapStr,
                                        XKB_KEYMAP_FORMAT_TEXT_V1,
                                        XKB_KEYMAP_COMPILE_NO_FLAGS);
        ::munmap(mapStr, size);
        ::close(fd);

        ONYX_ASSERT(keymap != nullptr, "Wayland: Failed to compile keymap");

        state = ::xkb_state_new(keymap);
        if (!state)
        {
            ::xkb_keymap_unref(keymap);
            ONYX_ASSERT(false, "Wayland: Failed to create XKB state");
        }
            
        // Look up the preferred locale, falling back to "C" as default.
        locale = ::getenv("LC_ALL");
        if (!locale)
            locale = ::getenv("LC_CTYPE");
        if (!locale)
            locale = ::getenv("LANG");
        if (!locale)
            locale = "C";

        composeTable = ::xkb_compose_table_new_from_locale(xkb.Context, locale, XKB_COMPOSE_COMPILE_NO_FLAGS);
        if (composeTable)
        {
            composeState = ::xkb_compose_state_new(composeTable, XKB_COMPOSE_STATE_NO_FLAGS);
            ::xkb_compose_table_unref(composeTable);
            if (composeState)
                xkb.ComposeState = composeState;
            else
                ONYX_LOG_ERROR("Wayland: Failed to create XKB compose state");
        }
        else
        {
            ONYX_LOG_ERROR( "Wayland: Failed to create XKB compose table");
        }
        xkb.SetKeymap(keymap);
        xkb.SetState(state);

        xkb.ControlIndex  = ::xkb_keymap_mod_get_index(keymap, "Control");
        xkb.AltIndex      = ::xkb_keymap_mod_get_index(keymap, "Mod1");
        xkb.ShiftIndex    = ::xkb_keymap_mod_get_index(keymap, "Shift");
        xkb.SuperIndex    = ::xkb_keymap_mod_get_index(keymap, "Mod4");
        xkb.CapsLockIndex = ::xkb_keymap_mod_get_index(keymap, "Lock");
        xkb.NumLockIndex  = ::xkb_keymap_mod_get_index(keymap, "Mod2");
    }

    /*static*/ void WaylandKeyboard::OnEnterSurface(void* instance, wl_keyboard* keyboard, onyxU32 serial, wl_surface* surface, wl_array* /*keys*/)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(keyboard);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(surface);
    }

    /*static*/ void WaylandKeyboard::OnLeaveSurface(void* instance, wl_keyboard* keyboard, onyxU32 serial, wl_surface* surface)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(keyboard);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(surface);
    }

    /*static*/ void WaylandKeyboard::OnKeyChange(void* instance, wl_keyboard* keyboard, onyxU32 /*serial*/, onyxU32 /*time*/, onyxU32 wayland_key, onyxU32 state)
    {
        WaylandKeyboard& keyboardInstance = *reinterpret_cast<WaylandKeyboard*>(instance);
        ONYX_ASSERT(keyboardInstance.m_Input != nullptr);

        WaylandInput& waylandInput = *keyboardInstance.m_Input;
        WaylandPlatformContext& context = waylandInput.GetContext();
        Input::InputSystem& inputSystem = context.GetInputSystem();

        Input::KeyboardEvent keyboardEvent;
        keyboardEvent.Id = state == 0 ? Input::InputEventType::KeyUp : Input::InputEventType::KeyDown;
        keyboardEvent.Key = WaylandKeyToKey(wayland_key);
        keyboardEvent.State = state != 0;
        keyboardEvent.Char = GetInputChar(context.GetXkb(), wayland_key);

        inputSystem.AddEvent(keyboardEvent);
    }

    /*static*/ void WaylandKeyboard::OnModifierChange(void* instance, wl_keyboard* keyboard, onyxU32 serial, onyxU32 mods_depressed, onyxU32 mods_latched, onyxU32 mods_locked, onyxU32 group)
    {
        ONYX_UNUSED(instance);
        ONYX_UNUSED(keyboard);
        ONYX_UNUSED(serial);
        ONYX_UNUSED(mods_depressed);
        ONYX_UNUSED(mods_latched);
        ONYX_UNUSED(mods_locked);
        ONYX_UNUSED(group);
    }
}