#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>

namespace Onyx::Localization
{
   void InitLocalization(const LocalizationModule& localizationModule)
   {
#if ONYX_IS_EDITOR
       // Generic
       Generic::Name = localizationModule.GetLocalized("generic.name");
       Generic::Add = localizationModule.GetLocalized("generic.add");
       Generic::Create = localizationModule.GetLocalized("generic.create");
       Generic::Duplicate = localizationModule.GetLocalized("generic.duplicate");
       Generic::Delete = localizationModule.GetLocalized("generic.delete");
       Generic::Search = localizationModule.GetLocalized("generic.search");
       Generic::Rename = localizationModule.GetLocalized("generic.rename");
       Generic::None = localizationModule.GetLocalized("generic.none");
       Generic::Invalid = localizationModule.GetLocalized("generic.invalid");
       
       Generic::Open = localizationModule.GetLocalized("generic.open");
       Generic::Save = localizationModule.GetLocalized("generic.save");
       Generic::SaveAs = localizationModule.GetLocalized("generic.saveas");

       Generic::File = localizationModule.GetLocalized("generic.file");
       Generic::Type = localizationModule.GetLocalized("generic.type");

       Generic::Default = localizationModule.GetLocalized("generic.default");
       Generic::Visibility = localizationModule.GetLocalized("generic.visibility");
#else
       ONYX_UNUSED(localizationModule);
#endif
   }
}
