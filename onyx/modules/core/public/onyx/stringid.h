#pragma once

namespace Onyx
{
    template <typename T> requires std::is_integral_v<T>
    struct StringId
    {
        using IdType = T;
        static constexpr T Invalid = 0;

        constexpr StringId()
            : Id(T(Invalid))
#if !ONYX_IS_RETAIL
            , IdString("Invalid")
#endif
        {

        }

        constexpr StringId(T id)
            : Id(id)
#if !ONYX_IS_RETAIL
            , IdString("IdString not provided")
#endif
        {
        }

        StringId(const String& string)
            : Id(Hash::FNV1aHash<T>(string))
#if !ONYX_IS_RETAIL
            , IdString(string)
#endif
        {
        }

        constexpr StringId(StringView string)
            : Id(Hash::FNV1aHash<T>(string))
#if !ONYX_IS_RETAIL
            , IdString(string)
#endif
        {
        }

        template <onyxU64 N>
        constexpr StringId(const char(&str)[N])
            : StringId(StringView(str))
        {
        }

        template <onyxU64 N>
        consteval StringId(const CompileTimeString<N>& str)
            : StringId(str.Data)
        {
        }

        constexpr bool IsValid() const { return Id != Invalid; }

        constexpr bool operator==(onyxU64 other) const { return Id == other; }
        constexpr bool operator!=(onyxU64 other) const { return Id != other; }
         
        constexpr bool operator==(const StringId& other) const { return Id == other.Id; }
        constexpr bool operator!=(const StringId& other) const { return Id != other.Id; }

        T Id;
#if !ONYX_IS_RETAIL
        StringView IdString;
#endif
    };

    using StringId32 = StringId<onyxU32>;
    using StringId64 = StringId<onyxU64>;
}

namespace std
{
    template<>
    struct hash<Onyx::StringId32>
    {
        size_t operator()(const Onyx::StringId32& id) const noexcept
        {
            return id.Id;
        }
    };

    template<>
    struct hash<Onyx::StringId64>
    {
        size_t operator()(const Onyx::StringId64& id) const noexcept
        {
            return id.Id;
        }
    };

    template <>
    struct std::formatter<Onyx::StringId32> : std::formatter<std::string> {
        auto format(Onyx::StringId32 id, format_context& ctx) const {
            return std::format_to(ctx.out(), "{}", id.Id);
        }
    };

    template <>
    struct std::formatter<Onyx::StringId64> : std::formatter<std::string> {
        auto format(Onyx::StringId64 id, format_context& ctx) const {
            return std::format_to(ctx.out(), "{}", id.Id);
        }
    };
}