#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

struct wl_pointer;
struct wl_surface;

namespace Onyx::Platform::Wayland
{
	class Input;

    class Pointer
    {
	public:
        Pointer(Input& input, wl_pointer* pointer);
		~Pointer();

    private:
        static void OnEnterSurface(void* instance, wl_pointer* pointer, onyxU32 serial, wl_surface* surface, onyxS32 x, onyxS32 y);
        static void OnLeaveSurface(void* instance, wl_pointer* pointer, onyxU32 serial, wl_surface* surface);
        static void OnMove(void* instance, wl_pointer* pointer, onyxU32 time, onyxS32 x, onyxS32 y);
        static void OnButton(void* instance, wl_pointer* pointer, onyxU32 serial, onyxU32 time, onyxU32 button, onyxU32 state);
        static void OnAxis(void* instance, wl_pointer* pointer, onyxU32 time, onyxU32 axis, onyxS32 value);

    private:
        Input* m_Input = nullptr;
        wl_pointer* m_Pointer = nullptr;
    };
}

#endif