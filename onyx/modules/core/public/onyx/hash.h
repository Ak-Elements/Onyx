#pragma once

#include <onyx/string/compiletimestring.h>
#include <filesystem>

namespace Onyx::Hash
{
    ONYX_NO_DISCARD constexpr onyxU32 FNV1aHash32(const onyxU8* data, onyxU64 length, onyxU32 seed)
    {
        constexpr onyxU32 FNV_PRIME = 16777619u;
        constexpr onyxU32 OFFSET_BASIS = 2166136261u;

        seed ^= OFFSET_BASIS;
        for (onyxU64 i = 0; i < length; ++i)
        {
            seed ^= static_cast<onyxU32>(data[i]);
            seed *= FNV_PRIME;
        }
        return seed;
    }

    ONYX_NO_DISCARD constexpr onyxU32 FNV1aHash32(const char* data, onyxU64 length, onyxU32 seed)
    {
        constexpr onyxU32 FNV_PRIME = 16777619u;
        constexpr onyxU32 OFFSET_BASIS = 2166136261u;

        seed ^= OFFSET_BASIS;
        for (onyxU64 i = 0; i < length; ++i)
        {
            if (*data == 0)
                break;

            seed ^= *data++;
            seed *= FNV_PRIME;
        }
        return seed;
    }

    ONYX_NO_DISCARD constexpr onyxU64 FNV1aHash64(const char* data, onyxU64 length, onyxU64 seed)
    {
        constexpr onyxU64 FNV_PRIME = 1099511628211ull;
        constexpr onyxU64 OFFSET_BASIS = 14695981039346656037ull;

        seed ^= OFFSET_BASIS;
        for (onyxU64 i = 0; i < length; ++i)
        {
            if (*data == 0)
                break;

            seed ^= *data++;
            seed *= FNV_PRIME;
        }
        return seed;
    }

    ONYX_NO_DISCARD constexpr onyxU64 FNV1aHash64(const onyxU8* data, onyxU64 length, onyxU64 seed)
    {
        constexpr onyxU64 FNV_PRIME = 1099511628211ull;
        constexpr onyxU64 OFFSET_BASIS = 14695981039346656037ull;

        seed ^= OFFSET_BASIS;
        for (onyxU64 i = 0; i < length; ++i)
        {
            seed ^= static_cast<onyxU64>(data[i]);
            seed *= FNV_PRIME;
        }
        return seed;
    }

    template <typename T> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const String& string, T seed)
    {
        if constexpr (std::is_same_v<T, onyxU64>)
        {
            return FNV1aHash64(string.data(), static_cast<onyxU64>(string.size()), seed);
        }
        else
        {
            return FNV1aHash32(string.data(), static_cast<onyxU64>(string.size()), seed);
        }
    }

    template <typename T> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const String& string)
    {
        return FNV1aHash<T>(string, T{ 0 });
    }


    template <typename T> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const StringView& string, T seed)
    {
        if constexpr (std::is_same_v<T, onyxU64>)
        {
            return FNV1aHash64(string.data(), static_cast<onyxU64>(string.size()), seed);
        }
        else
        {
            return FNV1aHash32(string.data(), static_cast<onyxU64>(string.size()), seed);
        }
    }

    template <typename T> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const StringView& string)
    {
        return FNV1aHash<T>(string, T{ 0 });
    }

    template <typename T, onyxU64 N> requires std::is_integral_v<T>
    consteval ONYX_NO_DISCARD T FNV1aHash(const CompileTimeString<N>& string, T seed)
    {
        if constexpr (std::is_same_v<T, onyxU64>)
        {
            return FNV1aHash64(string.data(), string.size(), seed);
        }
        else
        {
            return FNV1aHash32(string.data(), string.size(), seed);
        }
    }

    template <typename T, onyxU64 N> requires std::is_integral_v<T>
    ONYX_NO_DISCARD consteval T FNV1aHash(const CompileTimeString<N>& string)
    {
        return FNV1aHash<T>(string, T{ 0 });
    }

    template <typename T, onyxU64 N> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const char(&str)[N])
    {
        if constexpr (std::is_same_v<T, onyxU64>)
        {
            return FNV1aHash64(str, N, 0);
        }
        else
        {
            return FNV1aHash32(str, N, 0);
        }
    }

    template <typename T> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const std::filesystem::path& path)
    {
        return FNV1aHash<T>(path.string());
    }

    template <typename T, typename U> requires std::is_integral_v<T>
    ONYX_NO_DISCARD constexpr T FNV1aHash(const U& obj, T seed)
    {
        if constexpr (std::is_same_v<T, onyxU64>)
        {
            return FNV1aHash64(reinterpret_cast<const onyxU8*>(&obj), static_cast<onyxU64>(sizeof(U)), seed);
        }
        else
        {
            return FNV1aHash32(reinterpret_cast<const onyxU8*>(&obj), static_cast<onyxU64>(sizeof(U)), seed);
        }
    }

    template <typename T, typename U> requires std::is_integral_v<T>
    constexpr T FNV1aHash(const U& obj)
    {
        return FNV1aHash<T, U>(obj, T{ 0 });
    }

    ONYX_NO_DISCARD constexpr onyxU32 CRC32(const char* data, onyxU64 len, onyxU32 crc = 0);
    ONYX_NO_DISCARD constexpr onyxU32 CRC32(const StringView& str, onyxU32 crc = 0);
}
