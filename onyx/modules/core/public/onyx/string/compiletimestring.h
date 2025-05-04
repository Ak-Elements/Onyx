#pragma once

namespace Onyx
{
    template <onyxU64 N>
    struct CompileTimeString
    {
        consteval CompileTimeString() = default;
        consteval CompileTimeString(const char(&str)[N + 1])
        {
            for (onyxU64 i = 0; i < N + 1; ++i)
            {
                Data[i] = str[i];
            }
        }

        consteval onyxU64 size() const { return N; }
        consteval const char* data() const { return &Data[0]; }
        consteval StringView string_view() const { return { &Data[0], N }; }

        template <onyxU64 OtherN>
        consteval CompileTimeString<N + OtherN> operator+(const CompileTimeString<OtherN>& other) const
        {
            CompileTimeString<N + OtherN> result;
            for (int i = 0; i < N; ++i)
            {
                result.Data[i] = Data[i];
            }

            for (int i = 0; i < OtherN; ++i)
            {
                result.Data[N + i] = other.Data[i];
            }

            result.Data[N + OtherN] = '\0';

            return result;
        }

        template <onyxU64 OtherN>
        consteval auto operator+(const char(&other)[OtherN]) const
        {
            CompileTimeString<OtherN - 1> otherCompileTimeStr(other);
            return *this + otherCompileTimeStr;
        }

        char Data[N + 1];// Array<char, N + 1> Data;
    };

    template <onyxU64 N>
    CompileTimeString(const char(&str)[N]) -> CompileTimeString<N - 1>;
}