#pragma once
#include <onyx/assets/asset.h>
#include <onyx/localization/localizedstring.h>

namespace onyx::localization {
class GetTextLocalizationDatabase : public assets::Asset< GetTextLocalizationDatabase > {
  public:
    static constexpr StringId32 TypeId{ "onyx::localization::assets::GetTextLocalizationDatabase" };
    static StringId32 getTypeId() { return TypeId; }

    HashMap< LocalizationId, DynamicArray< String > >& getDatabase() { return m_localeDatabase; }
    const HashMap< LocalizationId, DynamicArray< String > >& getDatabase() const { return m_localeDatabase; }

    void setPluralFunction( int32_t( pluralFunction )( int32_t ) ) { m_pluralFunction = pluralFunction; }
    const InplaceFunction< int32_t( int32_t ) >& getPluralFunction() const { return m_pluralFunction; }

  private:
    HashMap< LocalizationId, DynamicArray< String > > m_localeDatabase;
    InplaceFunction< int32_t( int32_t ) > m_pluralFunction;
};
} // namespace onyx::localization
