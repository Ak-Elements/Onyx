#pragma once

namespace Onyx::Localization
{
    class ILocalizationBackend;

    struct LocalizationId
    {
        StringId32 Id;
    };

    struct LocalizedString
    {
        LocalizedString() = default;
        LocalizedString(LocalizationId id, Optional<StringView> localizedString, ILocalizationBackend& backend);
        LocalizedString(LocalizationId id, Optional<StringView> localizedString, onyxS32 count, ILocalizationBackend& backend);

        StringView Get() const;

    private:
        void OnLocalizationChanged(ILocalizationBackend& backend);

    private:
        LocalizationId m_LocalizationId;
        Optional<StringView> m_LocalizedString;
        onyxS32 m_Count;
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
