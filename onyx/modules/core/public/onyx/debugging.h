#pragma once

namespace Onyx
{
    namespace Internal
    {
        void Breakpoint();
    }

    inline void Breakpoint()
    {
        #if ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

        #if __has_builtin(__builtin_debugtrap) // vvv __has_builtin(__builtin_debugtrap) vvv

            __builtin_debugtrap();

        #elif ONYX_IS_MSVC // ^^^ __has_builtin(__builtin_debugtrap) ^^^ || vvv ONYX_IS_MSVC vvv

            __debugbreak();

        #else // ^^^ ONYX_IS_MSVC ^^^ || vvv !__has_builtin(__builtin_debugtrap) && !ONYX_IS_MSVC vvv

            Internal::Breakpoint();

        #endif // ^^^ !__has_builtin(__builtin_debugtrap) && !ONYX_IS_MSVC ^^^

        #endif // ^^^ ONYX_IS_DEBUG ^^^
    }

    bool IsDebuggerPresent();

    inline void BreakpointIfDebugging()
    {
        #if ONYX_IS_DEBUG

        if (IsDebuggerPresent())
        {
            Breakpoint();
        }

        #endif // ^^^ ONYX_IS_DEBUG ^^^
    }
}
