#pragma once

namespace Onyx::Localization
{
    class ILocalizationBackend;

    struct LocalizationId
    {
        LocalizationId() = default;

        LocalizationId(StringId32 id)
            : Context(0)
            , Id(id)
        {
        }

        LocalizationId(StringId32 context, StringId32 id)
            : Context(context)
            , Id(id)
        {
        }

        template <onyxU64 N>
        consteval LocalizationId(const CompileTimeString<N>& str)
            : Context(0)
            , Id(str)
        {
        }


        template <onyxU64 N, onyxU64 M>
        consteval LocalizationId(const CompileTimeString<N>& contextId, const CompileTimeString<M>& localizationId)
            : Context(contextId)
            , Id(localizationId)
        {
        }

        template <onyxU64 N>
        consteval LocalizationId(const char(&str)[N])
            : Id(StringView(str, StringView::traits_type::length(str)))
        {
        }

        template <onyxU64 N, onyxU64 M>
        consteval LocalizationId(const char(&contextId)[N], const char(&localizationId)[N])
            : Context(StringView(contextId, StringView::traits_type::length(contextId)))
            , Id(StringView(localizationId, StringView::traits_type::length(localizationId)))
        {
        }

        constexpr bool operator==(const LocalizationId& other) const { return Context == other.Context && Id == other.Id; }
        constexpr bool IsValid() const { return Id.IsValid(); }

        StringId32 Context;
        StringId32 Id;
    };

    // TODO: Investigate if caching the localized string would be better for performance
    // Problem with caching is, how to realize count for plural forms
    struct LocalizedString
    {
        LocalizedString() = default;
        LocalizedString(LocalizationId id, const ILocalizationBackend& backend);

        StringView Get() const;
        StringView Get(onyxS32 count) const;

        bool IsValid() const { return m_LocalizationId.IsValid(); }

    private:
        LocalizationId m_LocalizationId;
        const ILocalizationBackend* m_LocalizationBackend = nullptr;
    };
}

template <>
struct std::formatter<Onyx::Localization::LocalizedString> : std::formatter<std::string>
{
    auto format(Onyx::Localization::LocalizedString localizedString, format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{}", localizedString.Get());
    }
};

template<>
struct std::hash<Onyx::Localization::LocalizationId>
{
    size_t operator()(const Onyx::Localization::LocalizationId& id) const noexcept
    {
        Onyx::onyxU64 hash = Onyx::Hash::FNV1aHash(id.Id.GetId(), 0);
        return Onyx::Hash::FNV1aHash(id.Context.GetId(), hash);
    }
};

