#pragma once

#include <onyx/serialize/serialization.h>

namespace onyx
{
    class Serializer;

    class StringIdCache
    {
    public:
        ~StringIdCache()
        {
            
        }
        StringView Store(StringView string)
        {
            Optional<StringView> cachedString = TryGet(string);
            if (cachedString.has_value())
            {
                return cachedString.value();
            }

            std::lock_guard lock(m_Mutex);
            cachedString = TryGet(string);
            if (cachedString.has_value())
            {
                return cachedString.value();
            }

            return m_Cache.emplace_back(string);
        }
    private:
        Optional<StringView> TryGet(StringView string)
        {
            auto it = std::ranges::find_if(m_Cache, [&](const String& cachedString)
                {
                    return string == cachedString;
                });

            if (it == m_Cache.end())
            {
                return std::nullopt;
            }

            return { *it };
        }

    private:
        std::mutex m_Mutex;
        std::deque<String> m_Cache;
    };

    inline StringIdCache& GetIdCache()
    {
        static StringIdCache instance;
        return instance;
    }

    template <typename T> requires std::is_integral_v<T>
    struct StringId
    {
    private:
        friend struct Serialization<StringId>;

    public:
        using IdType = T;
        static constexpr T Invalid = 0;

        constexpr StringId()
#if ONYX_IS_RETAIL
            : Id(T(Invalid))
#else
            : m_IdString("Invalid")
            , m_Id(T(Invalid))
#endif
        {

        }

        constexpr StringId(T id)
#if ONYX_IS_RETAIL
            : Id(id)
#else
            : m_IdString("IdString not provided")
            , m_Id(id)
#endif
        {
        }

        constexpr StringId(StringView string)
#if ONYX_IS_RETAIL
            : Id(hash::FNV1aHash<T>(string))
#else
            : m_IdString([&]() -> StringView
                {
                    if (std::is_constant_evaluated())
                    {
                        return string; // Safe to store view
                    }
                    else
                    {
                        return GetIdCache().Store(string);
                    }
                }())
            , m_Id(hash::FNV1aHash<T>(string))
#endif
        {
        }

        template <onyxU64 N>
        constexpr StringId(const char(&str)[N])
            : StringId(StringView(str, StringView::traits_type::length(str)))
        {
        }

        template <onyxU64 N>
        consteval StringId(const CompileTimeString<N>& str)
            : StringId(str.string_view())
        {
        }

#if !ONYX_IS_RETAIL
        StringId(T id, const String& string)
            : m_IdString(GetIdCache().Store(string))
            , m_Id(id)
        {
        }

        StringId(T id, StringView string)
            : m_IdString(GetIdCache().Store(string))
            , m_Id(id)
            
        {
        }

#endif

        constexpr operator T() const { return m_Id; }

        constexpr bool IsValid() const { return m_Id != Invalid; }
        constexpr void Reset() { m_Id = Invalid; m_IdString = ""; }

        constexpr bool operator==(const StringId& other) const { return m_Id == other.m_Id; }
        constexpr bool operator!=(const StringId& other) const { return m_Id != other.m_Id; }

        constexpr T GetId() const { return m_Id; }
        constexpr StringView GetString() const { return m_IdString; }

    private:
#if !ONYX_IS_RETAIL
        StringView m_IdString;
#endif
        T m_Id;
    };

    using StringId32 = StringId<onyxU32>;
    using StringId64 = StringId<onyxU64>;

    template <typename T>
    constexpr bool IsStringId = is_specialization_of_v<StringId, T>;

    consteval StringId32 operator""_id32(const char* deg, std::size_t len)
    {
        return StringId32(StringView(deg, len));
    }

    consteval StringId64 operator""_id64(const char* deg, std::size_t len)
    {
        return StringId64(StringView(deg, len));
    }

    template <>
    struct Serialization<StringId32>
    {
        static bool Serialize(Serializer& serializer, const StringId32& id);
        static bool Deserialize(const Deserializer& deserializer, StringId32& id);
    };

    template <>
    struct Serialization<StringId64>
    {
        static bool Serialize(Serializer& serializer, const StringId64& id);
        static bool Deserialize(const Deserializer& deserializer, StringId64& id);
    };
}

namespace std
{
    template<>
    struct hash<onyx::StringId32>
    {
        size_t operator()(const onyx::StringId32& id) const noexcept
        {
            return id.GetId();
        }
    };

    template<>
    struct hash<onyx::StringId64>
    {
        size_t operator()(const onyx::StringId64& id) const noexcept
        {
            return id.GetId();
        }
    };

    template <>
    struct formatter<onyx::StringId32> : std::formatter<std::string>
    {
        auto format(onyx::StringId32 id, format_context& ctx) const
        {
#if ONYX_IS_RETAIL
            return std::format_to(ctx.out(), "{}({})", id.GetId());
#else
            return std::format_to(ctx.out(), "{}({})", id.GetId(), id.GetString());
#endif
        }
    };

    template <>
    struct formatter<onyx::StringId64> : std::formatter<std::string>
    {
        auto format(onyx::StringId64 id, format_context& ctx) const
        {
#if ONYX_IS_RETAIL
            return std::format_to(ctx.out(), "{}", id.GetId());
#else
            return std::format_to(ctx.out(), "{}({})", id.GetId(), id.GetString());
#endif
        }
    };
}