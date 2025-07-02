#include <onyx/ui/ui_localization.h>

#include <onyx/localization/localizationmodule.h>

namespace Onyx::Localization::Ui
{
    void InitLocalization(const LocalizationModule& localizationModule)
    {
#if ONYX_IS_EDITOR
        AssetSelector::Modal::Title = localizationModule.GetLocalized("ui.assetselector.modal.title");
#else
        ONYX_UNUSED(localizationModule);
#endif
    }
}
