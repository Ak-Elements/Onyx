#pragma once

#define ONYX_SAFE_DELETE( ptr )                                                                                        \
    delete ptr;                                                                                                        \
    ptr = nullptr

#ifndef ONYX_ALIGN
#define ONYX_ALIGN( v ) alignas( v )
#endif
