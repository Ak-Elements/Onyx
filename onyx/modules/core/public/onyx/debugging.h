#pragma once

#include <version>

namespace onyx {

#if defined( __cpp_lib_debugging ) && __cplusplus >= 202400 // vvv __cpp_lib_debugging vvv

#include <debugging>

inline void breakpoint() {
    std::breakpoint();
}

inline bool isDebuggerPresent() {
    return std::is_debugger_present();
}

inline void breakpointIfDebugging() {
    std::breakpoint_if_debugging()
}

#else             // ^^^ __cpp_lib_debugging ^^^ || vvv !__cpp_lib_debugging vvv

namespace internal {
void breakpoint();
}

bool isDebuggerPresent();

inline void breakpoint() {
#if ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

#if __has_builtin( __builtin_debugtrap ) // vvv __has_builtin(__builtin_debugtrap) vvv
    __builtin_debugtrap();
#elif ONYX_IS_MSVC                       // ^^^ __has_builtin(__builtin_debugtrap) ^^^ || vvv ONYX_IS_MSVC vvv
    __debugbreak();
#else  // ^^^ ONYX_IS_MSVC ^^^ || vvv !__has_builtin(__builtin_debugtrap) && !ONYX_IS_MSVC vvv
    internal::breakpoint();
#endif // ^^^ !__has_builtin(__builtin_debugtrap) && !ONYX_IS_MSVC ^^^
#endif // ^^^ ONYX_IS_DEBUG ^^^
}

inline void breakpointIfDebugging() {
#if ONYX_IS_DEBUG
    if( isDebuggerPresent() ) {
        breakpoint();
    }
#endif // ^^^ ONYX_IS_DEBUG ^^^
}

#endif // ^^^ !__cpp_lib_debugging ^^^

} // namespace onyx
