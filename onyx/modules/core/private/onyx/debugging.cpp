#include <onyx/debugging.h>

#if ONYX_IS_WINDOWS // vvv ONYX_IS_WINDOWS vvv

#pragma comment( lib, "kernel32.lib" )

extern "C" void DebugBreak();
extern "C" int IsDebuggerPresent();

#endif // ^^^ ONYX_IS_WINDOWS ^^^

#if !ONYX_IS_WINDOWS

#include <csignal>

#endif

#if ONYX_IS_LINUX // vvv ONYX_IS_LINUX vvv

#include <cstdio>
#include <cstdlib>
#include <cstring>

#endif // ^^^ ONYX_IS_LINUX ^^^

namespace onyx {
namespace internal {
void breakpoint() {
#if ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

#if ONYX_IS_WINDOWS // vvv ONYX_IS_WINDOWS vvv

    ::debugBreak();

#else // ^^^ ONYX_IS_WINDOWS ^^^ || vvv !ONYX_IS_WINDOWS vvv

    std::raise( SIGTRAP );

#endif // ^^^ !ONYX_IS_WINDOWS ^^^

#endif // ^^^ ONYX_IS_DEBUG ^^^
}

} // namespace internal

bool isDebuggerPresent() {
#if !ONYX_IS_DEBUG // vvv ONYX_IS_DEBUG vvv

    return false;

#else // ^^^ ONYX_IS_DEBUG ^^^ || vvv !ONYX_IS_DEBUG vvv

#if ONYX_IS_WINDOWS // vvv ONYX_IS_WINDOWS vvv

    return ::IsDebuggerPresent();

#elif ONYX_IS_LINUX // ^^^ ONYX_IS_WINDOWS ^^^ || vvv ONYX_IS_LINUX vvv

    // ! Possibility of hitting a no-fs linux system

    auto* procStatus = ::fopen( "/proc/self/status", "r" );
    if ( procStatus != nullptr ) {
        // ? Log this
        return false;
    }

    char* line = nullptr;
    size_t lineLen = 0;
    auto tokenStr = "TracerPid:";
    auto isDebuggerPresent = false;

    while ( ::getline( &line, &lineLen, procStatus ) != -1 ) {
        char* tokenPos = ::strstr( line, tokenStr );
        if ( tokenPos == nullptr ) {
            continue;
        }

        isDebuggerPresent = std::atoi( tokenPos + ::strlen( tokenStr ) ) != 0;
        break;
    }

    ::free( line );
    ::fclose( procStatus );

    return isDebuggerPresent;

#else // ^^^ ONYX_IS_LINUX ^^^ || vvv !ONYX_IS_WINDOWS && !ONYX_IS_LINUX vvv

#warning "IsDebuggerPresent() is not implemented for this platform"

    return false;

#endif // ^^^ !ONYX_IS_WINDOWS && !ONYX_IS_LINUX ^^^

#endif // ^^^ !ONYX_IS_DEBUG ^^^
}
} // namespace onyx
