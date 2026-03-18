#pragma once

namespace onyx {
namespace internal {
void breakpoint();
}

bool isDebuggerPresent();

inline void breakpoint() {
#if ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

#if __has_builtin( __builtin_debugtrap ) // vvv __has_builtin(__builtin_debugtrap) vvv
    __builtin_debugtrap();
#elif ONYX_IS_MSVC // ^^^ __has_builtin(__builtin_debugtrap) ^^^ || vvv ONYX_IS_MSVC vvv
    __debugbreak();
#else // ^^^ ONYX_IS_MSVC ^^^ || vvv !__has_builtin(__builtin_debugtrap) && !ONYX_IS_MSVC vvv
    Internal::Breakpoint();
#endif // ^^^ !__has_builtin(__builtin_debugtrap) && !ONYX_IS_MSVC ^^^
#endif // ^^^ ONYX_IS_DEBUG ^^^
}

inline void breakpointIfDebugging() {
#if ONYX_IS_DEBUG
    if ( isDebuggerPresent() ) {
        breakpoint();
    }
#endif // ^^^ ONYX_IS_DEBUG ^^^
}
} // namespace onyx
