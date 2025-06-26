#include <onyx/localization/localizationbackend.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    LocalizedString::LocalizedString(StringId32 id, ILocalizationBackend& backend)
        : m_LocalizationId(id)
        , m_LocalizedString(backend.Localize(id))
    {
    }

    LocalizedString::LocalizedString(StringId32 id, onyxS32 count, ILocalizationBackend& backend)
        : m_LocalizationId(id)
        , m_LocalizedString(backend.Localize(id, count))
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
            return Format::Format("!!!{}:{}", m_LocalizationId.GetString(), m_LocalizationId.GetId());
        }
#endif
    }
}
