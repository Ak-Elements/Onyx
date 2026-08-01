#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>

namespace onyx::localization {
void InitLocalization( [[maybe_unused]] const LocalizationModule& localizationModule ) {
#if ONYX_IS_EDITOR
    // Generic
    generic::Name = localizationModule.getLocalized( "generic.name" );
    generic::Add = localizationModule.getLocalized( "generic.add" );
    generic::Create = localizationModule.getLocalized( "generic.create" );
    generic::Duplicate = localizationModule.getLocalized( "generic.duplicate" );
    generic::Delete = localizationModule.getLocalized( "generic.delete" );
    generic::Search = localizationModule.getLocalized( "generic.search" );
    generic::Rename = localizationModule.getLocalized( "generic.rename" );
    generic::None = localizationModule.getLocalized( "generic.none" );
    generic::Invalid = localizationModule.getLocalized( "generic.invalid" );
    generic::MoveUp = localizationModule.getLocalized( "generic.move_up" );
    generic::MoveDown = localizationModule.getLocalized( "generic.move_down" );
    generic::InsertBefore = localizationModule.getLocalized( "generic.insert_before" );
    generic::InsertAfter = localizationModule.getLocalized( "generic.insert_after" );

    generic::Open = localizationModule.getLocalized( "generic.open" );
    generic::Save = localizationModule.getLocalized( "generic.save" );
    generic::SaveAs = localizationModule.getLocalized( "generic.saveas" );

    generic::File = localizationModule.getLocalized( "generic.file" );
    generic::Type = localizationModule.getLocalized( "generic.type" );

    generic::Default = localizationModule.getLocalized( "generic.default" );
    generic::Visibility = localizationModule.getLocalized( "generic.visibility" );

    generic::AddCollectionEntry = localizationModule.getLocalized( "generic.collection.add" );
    generic::CollectionEntry = localizationModule.getLocalized( "generic.collection.entry" );
    generic::CollectionSize = localizationModule.getLocalized( "generic.collection.size" );
#endif
}
} // namespace onyx::localization
