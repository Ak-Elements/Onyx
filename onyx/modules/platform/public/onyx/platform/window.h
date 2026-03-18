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

namespace onyx::platform
{
#if ONYX_IS_WINDOWS
using Window = windows::Window;
#elif ONYX_IS_LINUX && ONYX_USE_WAYLAND
using Window = wayland::Window;
#else
//static_assert(false, "Unsupported window library.");
#endif
}