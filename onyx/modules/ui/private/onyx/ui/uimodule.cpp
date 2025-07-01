#include <onyx/ui/uimodule.h>

#include <onyx/ui/ui_localization.h>

namespace Onyx::Ui
{
    void UiModule::Init(Localization::LocalizationModule& localizationModule)
    {
        Localization::Ui::InitLocalization(localizationModule);
    }
}
