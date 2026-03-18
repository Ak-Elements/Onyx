#include <onyx/localization/localizedstring.h>
#include <onyx/localization/localizationbackend.h>

namespace onyx::localization
{
    LocalizedString::LocalizedString(LocalizationId id, const ILocalizationBackend& backend)
        : m_LocalizationId(id)
        , m_LocalizationBackend(&backend)
    {
    }
    
    StringView LocalizedString::Get() const
    {
        Optional<StringView> localized = m_LocalizationBackend->GetLocalized(m_LocalizationId);
#if ONYX_IS_RETAIL
        return *localized;
#else
        if (localized.has_value())
        {
            return *localized;
        }
        else
        {
            return format::Format("!!!{}:{}", m_LocalizationId.Id.GetString(), m_LocalizationId.Id.GetId());
        }
#endif
    }

    StringView LocalizedString::Get(onyxS32 count) const
    {
        Optional<StringView> localized = m_LocalizationBackend->GetLocalized(m_LocalizationId, count);
#if ONYX_IS_RETAIL
        return *localized;
#else
        if (localized.has_value())
        {
            return *localized;
        }
        else
        {
            return format::Format("!!!{}:{}", m_LocalizationId.Id.GetString(), m_LocalizationId.Id.GetId());
        }
#endif
    }
}
