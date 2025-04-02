#pragma once

namespace Onyx
{
    template <onyxU64 N>
    struct CompileTimeString
    {
        constexpr CompileTimeString(const char(&str)[N + 1])
        {
            for (onyxU64 i = 0; i < N + 1; ++i)
            {
                Data[i] = str[i];
            }
        }

        char Data[N + 1];
        constexpr onyxU64 size() const { return N; }
        constexpr const char* data() const { return &Data[0]; }

    };

    template <onyxU64 N>
    CompileTimeString(const char(&str)[N]) -> CompileTimeString<N - 1>;
}