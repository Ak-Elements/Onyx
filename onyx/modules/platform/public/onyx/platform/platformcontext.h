#pragma once

#if ONYX_IS_WINDOWS
#include <onyx/platform/windows/windowsplatformcontext.h>
#elif ONYX_IS_LINUX
#if ONYX_USE_WAYLAND
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>
#elif ONYX_USE_X11
#include <onyx/platform/linux/x11/x11platformcontext.h>
#endif
#else
//static_assert(false, "Unsupported window library.");
#endif

namespace onyx::platform
{
#if ONYX_IS_WINDOWS
using PlatformContext = windows::PlatformContext;
#elif ONYX_IS_LINUX
#if ONYX_USE_WAYLAND
using PlatformContext = wayland::WaylandPlatformContext;
#elif ONYX_USE_X11
using PlatformContext = x11::X11PlatformContext;
#endif
#else
//static_assert(false, "Unsupported window library.");
#endif
}