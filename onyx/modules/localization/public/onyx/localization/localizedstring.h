#pragma once

namespace Onyx::Localization
{
    class ILocalizationBackend;

    struct LocalizedString
    {
        LocalizedString() = default;
        LocalizedString(StringId32 id, ILocalizationBackend& backend);
        LocalizedString(StringId32 id, onyxS32 count, ILocalizationBackend& backend);

        StringView Get() const;

    private:
        void OnLocalizationChanged(ILocalizationBackend& backend);
    private:
        StringId32 m_LocalizationId;
        Optional<StringView> m_LocalizedString;
    };
}