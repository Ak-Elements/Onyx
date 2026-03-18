#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>

namespace onyx::localization
{
   void InitLocalization(const LocalizationModule& localizationModule)
   {
#if ONYX_IS_EDITOR
       // Generic
       generic::Name = localizationModule.GetLocalized("generic.name");
       generic::Add = localizationModule.GetLocalized("generic.add");
       generic::Create = localizationModule.GetLocalized("generic.create");
       generic::Duplicate = localizationModule.GetLocalized("generic.duplicate");
       generic::Delete = localizationModule.GetLocalized("generic.delete");
       generic::Search = localizationModule.GetLocalized("generic.search");
       generic::Rename = localizationModule.GetLocalized("generic.rename");
       generic::None = localizationModule.GetLocalized("generic.none");
       generic::Invalid = localizationModule.GetLocalized("generic.invalid");
       
       generic::Open = localizationModule.GetLocalized("generic.open");
       generic::Save = localizationModule.GetLocalized("generic.save");
       generic::SaveAs = localizationModule.GetLocalized("generic.saveas");

       generic::File = localizationModule.GetLocalized("generic.file");
       generic::Type = localizationModule.GetLocalized("generic.type");

       generic::Default = localizationModule.GetLocalized("generic.default");
       generic::Visibility = localizationModule.GetLocalized("generic.visibility");
#else
       ONYX_UNUSED(localizationModule);
#endif
   }
}
