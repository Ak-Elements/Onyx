#pragma once
#include <onyx/localization/localizationbackend.h>

namespace Onyx::Localization
{
    class PoLocalizationBackend : public ILocalizationBackend
    {
    public:
        LocalizedString Localize(LocalizedStringId id) const override;
    };
}
