#include <onyx/localization/backends/pobackend.h>
#include <onyx/stream/stringstream.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    void PoLocalizationBackend::Init(Assets::AssetSystem& assetSystem, const LocalizationSettings& localizationSettings)
    {
        String fileContent;
        FileSystem::OnyxFile file("project:/localization/test.po");
        file.ReadAll("project:/localization/test.po", fileContent);
        StringStream stringStream(fileContent);


        while (stringStream.IsEof() == false)
        {
            if (stringStream.ReadConditional("msgid"))
            {
                StringView localizationIdString;
                stringStream.ReadStringUntil(localizationIdString,'\n');

                bool hasMsgStr = stringStream.ReadConditional("msgstr");
                if (hasMsgStr == false)
                    return;

                StringView localizedText;
                stringStream.ReadStringUntil(localizedText, '\n');

                onyxU32 id = Hash::FNV1aHash(localizationIdString, 0);
                m_LocaleDatabase[LocalizedStringId(id)] = localizedText;
            }
        }
    }

    LocalizedString PoLocalizationBackend::Localize(LocalizedStringId id) const
    {
    }
}
