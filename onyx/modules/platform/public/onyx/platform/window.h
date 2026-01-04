#pragma once


#if ONYX_IS_WINDOWS
#include <onyx/platform/windows/windowswindow.h>
#elif ONYX_IS_LINUX
#if ONYX_USE_WAYLAND
#include <onyx/platform/linux/wayland/waylandwindow.h>
#endif
#else
//static_assert(false, "Unsupported window library.");
#endif

namespace Onyx::Platform
{
#if ONYX_IS_WINDOWS
using Window = Onyx::Platform::WindowsWindow;
#elif ONYX_IS_LINUX
#if ONYX_USE_WAYLAND
using Window = Onyx::Platform::WaylandWindow;
#endif
#else
//static_assert(false, "Unsupported window library.");
#endif
}