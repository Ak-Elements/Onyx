#include <onyx/ui/uimodule.h>

#include <onyx/ui/ui_localization.h>

namespace onyx::ui
{
    UiModule::UiModule(onyx::localization::LocalizationModule& localizationModule)
    {
        localization::InitLocalization(localizationModule);
    }
}
