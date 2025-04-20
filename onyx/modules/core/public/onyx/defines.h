#pragma once

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