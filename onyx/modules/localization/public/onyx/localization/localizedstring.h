#pragma once

namespace Onyx::Localization
{
    class ILocalizationBackend;

    enum class LocalizedStringId : onyxU32;

    struct LocalizedString
    {
        LocalizedString(ILocalizationBackend& backend);

    private:
        void OnLocalizationChanged(ILocalizationBackend& backend);
    private:
        LocalizedStringId m_LocalizationId;
        StringView m_LocalizedString;
    };
}