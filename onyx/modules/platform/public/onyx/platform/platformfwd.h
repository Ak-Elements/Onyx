#pragma once
namespace onyx::platform
{
#if ONYX_IS_WINDOWS

    namespace windows
    {
        class PlatformContext;
        class Window;
    }

    using Window = windows::Window;
    using PlatformContext = windows::PlatformContext;

#elif ONYX_IS_LINUX && ONYX_USE_WAYLAND

    namespace wayland
    {
        class WaylandPlatformContext;
        class Window;
    }

    using Window = wayland::Window;
    using PlatformContext = wayland::WaylandPlatformContext;
#elif ONYX_IS_LINUX && ONYX_USE_X11

    namespace x11
    {
        class X11PlatformContext;
        class Window;
    }

    using Window = x11::Window;
    using PlatformContext = x11::X11PlatformContext;
#else
    //static_assert(false, "Unsupported window library.");
#endif
}
