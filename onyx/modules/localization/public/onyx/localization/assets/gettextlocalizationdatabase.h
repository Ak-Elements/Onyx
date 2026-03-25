#pragma once
#include <onyx/assets/asset.h>
#include <onyx/localization/localizedstring.h>

namespace onyx::localization {
class GetTextLocalizationDatabase : public assets::Asset< GetTextLocalizationDatabase > {
  public:
    static constexpr StringId32 TypeId{ "onyx::localization::assets::GetTextLocalizationDatabase" };
    StringId32 GetTypeId() const { return TypeId; }

    HashMap< LocalizationId, DynamicArray< String > >& GetDatabase() { return m_LocaleDatabase; }
    const HashMap< LocalizationId, DynamicArray< String > >& GetDatabase() const { return m_LocaleDatabase; }

    void SetPluralFunction( int32_t( pluralFunction )( int32_t ) ) { m_PluralFunction = pluralFunction; }
    const InplaceFunction< int32_t( int32_t ) >& GetPluralFunction() const { return m_PluralFunction; }

  private:
    HashMap< LocalizationId, DynamicArray< String > > m_LocaleDatabase;
    InplaceFunction< int32_t( int32_t ) > m_PluralFunction;
};
} // namespace onyx::localization
