#pragma once

#include <onyx/localization/localizedstring.h>

namespace onyx::localization
{
    class LocalizationModule;
}

namespace onyx::ui::localization
{
#if ONYX_IS_EDITOR
    namespace asset_selector::modal
    {
        inline onyx::localization::LocalizedString Title;
    }
    
#endif

    void InitLocalization(const onyx::localization::LocalizationModule& localizationModule);
}