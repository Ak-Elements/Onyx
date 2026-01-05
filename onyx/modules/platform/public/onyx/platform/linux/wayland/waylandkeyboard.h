#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

struct wl_pointer;
struct wl_surface;
struct wl_keyboard;
struct wl_array;

namespace Onyx::Platform::Wayland
{
	class WaylandInput;

    class WaylandKeyboard
    {
	public:
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

#endif