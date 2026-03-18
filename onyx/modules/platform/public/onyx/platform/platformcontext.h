#pragma once

#if ONYX_IS_WINDOWS
#include <onyx/platform/windows/windowsplatformcontext.h>
#elif ONYX_IS_LINUX
#if ONYX_USE_WAYLAND
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>
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
#endif
#else
//static_assert(false, "Unsupported window library.");
#endif
}