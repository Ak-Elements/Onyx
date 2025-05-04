#pragma once

namespace Onyx
{
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
        using IdType = T;
        static constexpr T Invalid = 0;

        constexpr StringId()
#if ONYX_IS_RETAIL
            : Id(T(Invalid))
#else
            : IdString("Invalid")
            , Id(T(Invalid))
#endif
        {

        }

        constexpr StringId(T id)
#if ONYX_IS_RETAIL
            : Id(id)
#else
            : IdString("IdString not provided")
            , Id(id)
#endif
        {
        }

        constexpr StringId(StringView string)
#if ONYX_IS_RETAIL
            : Id(Hash::FNV1aHash<T>(string))
#else
            : IdString([&]() -> StringView
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
            , Id(Hash::FNV1aHash<T>(string))
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
            : IdString(GetIdCache().Store(string))
            , Id(id)
        {
        }

        StringId(T id, StringView string)
            : IdString(GetIdCache().Store(string))
            , Id(id)
            
        {
        }

#endif

        constexpr operator T() const { return Id; }

        constexpr bool IsValid() const { return Id != Invalid; }
         
        constexpr bool operator==(const StringId& other) const { return Id == other.Id; }
        constexpr bool operator!=(const StringId& other) const { return Id != other.Id; }

        constexpr T GetId() const { return Id; }
        constexpr StringView GetString() const { return IdString; }

    private:
#if !ONYX_IS_RETAIL
        StringView IdString;
#endif
        T Id;
    };

    using StringId32 = StringId<onyxU32>;
    using StringId64 = StringId<onyxU64>;

    consteval StringId32 operator""_id32(const char* deg, std::size_t len)
    {
        return StringId32(StringView(deg, len));
    }

    consteval StringId64 operator""_id64(const char* deg, std::size_t len)
    {
        return StringId64(StringView(deg, len));
    }
}

namespace std
{
    template<>
    struct hash<Onyx::StringId32>
    {
        size_t operator()(const Onyx::StringId32& id) const noexcept
        {
            return id.GetId();
        }
    };

    template<>
    struct hash<Onyx::StringId64>
    {
        size_t operator()(const Onyx::StringId64& id) const noexcept
        {
            return id.GetId();
        }
    };

    template <>
    struct std::formatter<Onyx::StringId32> : std::formatter<std::string>
    {
        auto format(Onyx::StringId32 id, format_context& ctx) const
        {
#if ONYX_IS_RETAIL
            return std::format_to(ctx.out(), "{}({})", id.GetId());
#else
            return std::format_to(ctx.out(), "{}({})", id.GetId(), id.GetString());
#endif
        }
    };

    template <>
    struct std::formatter<Onyx::StringId64> : std::formatter<std::string>
    {
        auto format(Onyx::StringId64 id, format_context& ctx) const
        {
#if ONYX_IS_RETAIL
            return std::format_to(ctx.out(), "{}", id.GetId());
#else
            return std::format_to(ctx.out(), "{}({})", id.GetId(), id.GetString());
#endif
        }
    };
}