#pragma once
namespace Onyx::Platform
{
#if ONYX_IS_WINDOWS

    namespace Windows
    {
        class PlatformContext;
        class Window;
    }

    using Window = Windows::Window;
    using PlatformContext = Windows::PlatformContext;

#elif ONYX_IS_LINUX && ONYX_USE_WAYLAND

    namespace Wayland
    {
        class WaylandPlatformContext;
        class Window;
    }

    using Window = Wayland::Window;
    using PlatformContext = Wayland::WaylandPlatformContext;

#else
    //static_assert(false, "Unsupported window library.");
#endif
}
