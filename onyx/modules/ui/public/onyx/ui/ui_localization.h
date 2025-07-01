#pragma once

#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    class LocalizationModule;

    namespace Ui
    {
#if ONYX_IS_EDITOR
        namespace AssetSelector
        {
            namespace Modal
            {
                inline LocalizedString Title;
            }
        }
#endif

        void InitLocalization(const LocalizationModule& localizationModule);

    }
}