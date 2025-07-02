#pragma once
#include <onyx/assets/asset.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    class GetTextLocalizationDatabase : public Assets::Asset<GetTextLocalizationDatabase>
    {
    public:
        static constexpr StringId32 TypeId{ "Onyx::Localization::Assets::GetTextLocalizationDatabase" };
        StringId32 GetTypeId() const { return TypeId; }

        HashMap<LocalizationId, DynamicArray<String>>& GetDatabase() { return m_LocaleDatabase; }
        const HashMap<LocalizationId, DynamicArray<String>>& GetDatabase() const { return m_LocaleDatabase; }

        void SetPluralFunction(onyxS32 (pluralFunction)(onyxS32)) { m_PluralFunction = pluralFunction; }
        const InplaceFunction<onyxS32(onyxS32)>& GetPluralFunction() const { return m_PluralFunction; }

    private:
        HashMap<LocalizationId, DynamicArray<String>> m_LocaleDatabase;
        InplaceFunction<onyxS32(onyxS32)> m_PluralFunction;
    };
}
