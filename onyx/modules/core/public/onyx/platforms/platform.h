#pragma once

#if ONYX_IS_WINDOWS
#include <onyx/platforms/windows/platform.h>
#else
static_assert(false, "Platform not supported");
#endif