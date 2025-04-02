#pragma once

#ifdef _MSC_VER
#define ONYX_IS_WINDOWS 1
#else
#define ONYX_IS_WINDOWS 0
#endif

#ifdef __ANDROID__
#define ONYX_IS_ANDROID 1
#else
#define ONYX_IS_ANDROID 0
#endif

#ifndef ONYX_USE_SDL2
#define ONYX_USE_SDL2 0
#endif

#define ONYX_UNUSED(v) (void)v
#define ONYX_SAFE_DELETE(ptr)   \
    delete ptr;                 \
    ptr = nullptr

#ifndef ONYX_NO_DISCARD
#define ONYX_NO_DISCARD [[nodiscard]]
#endif

#ifndef ONYX_ALIGN
#define ONYX_ALIGN(v) alignas(v)
#endif