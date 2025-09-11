#include <onyx/debugging.h>

#pragma comment(lib, "kernel32.lib")

extern "C" void DebugBreak();
extern "C" int IsDebuggerPresent();

#if !ONYX_IS_WINDOWS

#include <csignal>

#endif

namespace Onyx
{

    namespace Internal
    {

        void Breakpoint()
        {
            #if ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

            #if ONYX_IS_WINDOWS // vvv ONYX_IS_WINDOWS vvv

            ::DebugBreak();

            #else // ^^^ ONYX_IS_WINDOWS ^^^ || vvv !ONYX_IS_WINDOWS vvv

            std::raise(SIGTRAP);

            #endif // ^^^ !ONYX_IS_WINDOWS ^^^

            #endif // ^^^ ONYX_IS_DEBUG ^^^
        }

    }

    bool IsDebuggerPresent()
    {
        #if !ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

        return false;

        #else // ^^^ ONYX_IS_DEBUG ^^^ || vvv !ONYX_IS_DEBUG vvv

        #if ONYX_IS_WINDOWS // vvv ONYX_IS_WINDOWS vvv

        return ::IsDebuggerPresent();

        #else // ^^^ ONYX_IS_WINDOWS ^^^ || vvv !ONYX_IS_WINDOWS vvv

        #warning "IsDebuggerPresent() is not implemented for this platform"

        return false;

        #endif // ^^^ !ONYX_IS_WINDOWS ^^^

        #endif // ^^^ !ONYX_IS_DEBUG ^^^
    }

}
