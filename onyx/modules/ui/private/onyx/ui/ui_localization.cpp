#include <onyx/ui/ui_localization.h>

#include <onyx/localization/localizationmodule.h>

namespace onyx::ui::localization {
void InitLocalization( const onyx::localization::LocalizationModule& localizationModule ) {
#if ONYX_IS_EDITOR
    asset_selector::modal::Title = localizationModule.getLocalized( "ui.assetselector.modal.title" );
#else
    ONYX_UNUSED( localizationModule );
#endif
}
} // namespace onyx::ui::localization
