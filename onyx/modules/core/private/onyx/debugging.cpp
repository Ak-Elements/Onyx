#include <onyx/debugging.h>

#if ONYX_IS_WINDOWS // vvv ONYX_IS_WINDOWS vvv

#pragma comment(lib, "kernel32.lib")

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

        static bool is_debugger_present = [] {
          return ::IsDebuggerPresent();
        }();

#elif ONYX_IS_LINUX // ^^^ ONYX_IS_WINDOWS ^^^ || vvv ONYX_IS_LINUX vvv

        static bool is_debugger_present = [] {
            // ! Possibility of hitting a no-fs linux system
            auto *proc_status = ::fopen("/proc/self/status", "r");
            if (proc_status != nullptr) {
                // ? Log this
                return false;
            }

            char *line = nullptr;
            ::size_t line_len = 0;
            auto token_str = "TracerPid:";
            auto is_debugger_present = false;

            while (::getline(&line, &line_len, proc_status) != -1) {
                char *token_pos = ::strstr(line, token_str);
                if (token_pos == nullptr) {
                    continue;
                }

                is_debugger_present = ::atoi(token_pos + ::strlen(token_str)) != 0;
                break;
            }

            ::free(line);
            ::fclose(proc_status);

            return is_debugger_present;
        }();

#else // ^^^ ONYX_IS_LINUX ^^^ || vvv !ONYX_IS_WINDOWS && !ONYX_IS_LINUX vvv

#warning "IsDebuggerPresent() is not implemented for this platform"

        static bool is_debugger_present = false;

#endif // ^^^ !ONYX_IS_WINDOWS && !ONYX_IS_LINUX ^^^

        return is_debugger_present;

#endif // ^^^ !ONYX_IS_DEBUG ^^^
    }
}
