#pragma once

#include <onyx/input/keycodes.h>
#include <onyx/platform/platformsystem.h>

struct wl_pointer;
struct wl_surface;
struct wl_keyboard;
struct wl_array;

namespace Onyx::Platform
{
	class WaylandInput;

    class WaylandKeyboard
    {
	public:
        using OnKeyMapChangeSignalT = Signal<void(onyxU32, onyxS32, onyxU32)>;
        using OnKeySignalT = Signal<void(onyxU16, bool)>;
        using OnModifierSignalT = Signal<void(onyxU16, bool)>;

		WaylandKeyboard(WaylandInput& input, wl_keyboard* keyboard);
        ~WaylandKeyboard();
    
    private:
        static void OnKeyMap(void* inputInstance, wl_keyboard* keyboard, onyxU32 format, onyxS32 fd, onyxU32 size);
        static void OnEnterSurface(void* inputInstance, wl_keyboard* keyboard, onyxU32 serial, wl_surface* surface, wl_array* keys);
        static void OnLeaveSurface(void* inputInstance, wl_keyboard* keyboard, onyxU32 serial, wl_surface* surface);
        static void OnKeyChange(void* inputInstance, wl_keyboard* keyboard, onyxU32 serial, onyxU32 time, onyxU32 key, onyxU32 state);
        static void OnModifierChange(void* inputInstance, wl_keyboard* keyboard, onyxU32 serial, onyxU32 mods_depressed, onyxU32 mods_latched, onyxU32 mods_locked, onyxU32 group);

    private:
        WaylandInput* m_Input = nullptr;
        wl_keyboard* m_Keyboard = nullptr;
    };
}