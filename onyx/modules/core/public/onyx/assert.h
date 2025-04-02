#pragma once

#if ONYX_IS_DEBUG
#define ONYX_ASSERTS_ENABLED 1
#else
#define ONYX_ASSERTS_ENABLED 0
#endif

#if ONYX_ASSERTS_ENABLED

#include <onyx/inplacefunction.h>
#include <onyx/string/inplacestring.h>
#include <onyx/string/format.h>

namespace Onyx
{
    inline InplaceFunction<void(const StringView& message)> ErrorFunctor = [](const StringView& /*msg*/)
    {
    };

    template <typename... Args>
    bool Assert(const char* file, int line, const char* conditionString, std::format_string<Args...> fmt, Args&&... args)
    {
        InplaceString<512> finalMessage;

        constexpr onyxU32 formatArgsCount = sizeof...(args);
        InplaceString<250> messageStr;
        if constexpr (formatArgsCount > 0)
        {
            Format::FormatTo(messageStr, fmt, std::forward<Args>(args)...);
        }

        Format::FormatTo(finalMessage, "{}({}) : Assert failed({}): {} \n", file, line, conditionString, messageStr.GetData());

        return true;
    }

    template <typename... Args>
    bool Assert(const char* file, int line, const char* conditionString)
    {
        InplaceString<512> finalMessage;
        Format::FormatTo(finalMessage, "{}({}) : Assert failed({}) \n", file, line, conditionString);
        ErrorFunctor(finalMessage.GetData());
        return true;
    }
}

#if ONYX_IS_VISUAL_STUDIO
#define DEBUG_BREAK __debugbreak()
#elif IS_CLANG
#if ANDROID
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#else
#define DEBUG_BREAK() __builtin_debugtrap()
#endif
#elif IS_GCC// gcc
#define DEBUG_BREAK __builtin_trap()
#else
//static_assert(false, "Not supported on this platform yet");
#endif

#define ONYX_ASSERT(condition, ...)                                         \
    do {                                                                    \
        if (!(condition)) {                                                 \
            if(Onyx::Assert(__FILE__, __LINE__, #condition, ##__VA_ARGS__)) \
            {                                                               \
                DEBUG_BREAK;                                                \
                std::terminate();                                           \
            }                                                               \
        }                                                                   \
    } while (false)
#else
#define ONYX_ASSERT(condition, ...)
#endif