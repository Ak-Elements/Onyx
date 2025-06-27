#include <onyx/localization/localizationbackend.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    LocalizedString::LocalizedString(LocalizationId id, Optional<StringView> localizedString, ILocalizationBackend& /*backend*/)
        : m_LocalizationId(id)
        , m_LocalizedString(localizedString)
    {
    }

    LocalizedString::LocalizedString(LocalizationId id, Optional<StringView> localizedString, onyxS32 count, ILocalizationBackend& /*backend*/)
        : m_LocalizationId(id)
        , m_LocalizedString(localizedString)
        , m_Count(count)
    {
    }
    
    StringView LocalizedString::Get() const
    {
#if ONYX_IS_RETAIL
        return *m_LocalizedString;
#else
        if (m_LocalizedString.has_value())
        {
            return *m_LocalizedString;
        }
        else
        {
            return Format::Format("!!!{}:{}", m_LocalizationId.Id.GetString(), m_LocalizationId.Id.GetId());
        }
#endif
    }
}
