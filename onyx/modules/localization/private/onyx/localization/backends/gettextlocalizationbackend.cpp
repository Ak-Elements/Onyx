#include <onyx/localization/backends/gettextlocalizationbackend.h>
#include <onyx/stream/stringstream.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    namespace
    {
        constexpr StringView PO_HEADER_MSG_ID = "msgid \"\"\n";
        constexpr StringView PO_HEADER_MSG_STR = "msgstr \"\"\n";
        constexpr StringView PO_HEADER_PLURAL_FORMS = "plural-forms:";

        struct PluralRule
        {
            using PluralFunc = onyxS32(*)(onyxS32);

            StringView Rule;
            PluralFunc Function;
        };

        constexpr Array PluralFunctions =
        {
            // Note that the plural forms here shouldn't contain any spaces
            // Japanese, Vietnamese, Korean
            // Thai 
            PluralRule{ "nplurals=1;plural=0;", [](onyxS32) { return 0; } },

            // English, German, Dutch, Swedish, Danish, Norwegian, Faroese
            // Spanish, Portuguese, Italian
            // Greek
            // Bulgarian
            // Finnish, Estonian
            // Hebrew
            // Bahasa Indonesian
            // Esperanto
            // Hungarian
            // Turkish 
            PluralRule{ "nplurals=2;plural=(n!=1);", [](onyxS32 n) { return n == 1 ? 0 : 1; }  },

            /// Brazilian Portuguese, French 
            PluralRule{ "nplurals=2;plural=(n>1);", [](onyxS32 n) { return n > 1 ? 0 : 1; } },

            // Macedonian
            PluralRule{ "nplurals=2;plural=n==1||n%10==1?0:1;", [](onyxS32 n) { return (n == 1) || ((n % 10) == 1) ? 0 : 1; } },

            // Macedonian 2
            PluralRule{ "nplurals=2;plural=(n%10==1&&n%100!=11)?0:1;", [](onyxS32 n) { return ((n % 10) == 1) && ((n % 100) != 11) ? 0 : 1; } },

            // Latvian 
            PluralRule{ "nplurals=3;plural=n%10==1&&n%100!=11?0:n!=0?1:2);", [](onyxS32 n) { return ((n % 10) == 1) && ((n % 100) != 11) ? 0 : (n != 0) ? 1 : 2; } },

            // Gaeilge Irish
            PluralRule{ "nplurals=3;plural=n==1?0:n==2?1:2;", [](onyxS32 n) { return (n == 1) ? 0 : (n == 2) ? 1 : 2; } },

            // Lithuanian
            PluralRule{ "nplurals=3;plural=(n%10==1&&n%100!=11?0:n%10>=2&&(n%100<10||n%100>=20)?1:2);", [](onyxS32 n) { return ((n % 10) == 1) && ((n % 100) != 11) ? 0 : ((n % 10) >= 2) && (((n % 100) < 10) || ((n % 100) >= 20)) ? 1 : 2; }  },

            // Russian, Ukrainian, Belarusian, Serbian, Croatian 
            PluralRule{ "nplurals=3;plural=(n%10==1&&n%100!=11?0:n%10>=2&&n%10<=4&&(n%100<10||n%100>=20)?1:2);", [](onyxS32 n) { return ((n % 10) == 1) && ((n % 100) != 11) ? 0 : ((n % 10) >= 2) && ((n % 10) <= 4) && (((n % 100) < 10) || ((n % 100) >= 20)) ? 1 : 2; }},

            // Czech, Slovak 
            PluralRule{ "nplurals=3;plural=(n==1)?0:(n>=2&&n<=4)?1:2;", [](onyxS32 n) { return (n == 1) ? 0 : (n >= 2) && (n <= 4) ? 1 : 2; }  },

            // Polish
            PluralRule{ "nplurals=3;plural=(n==1?0:n%10>=2&&n%10<=4&&(n%100<10||n%100>=20)?1:2);", [](onyxS32 n) { return (n == 1) ? 0 : ((n % 10) >= 2) && ((n % 10) <= 4) && (((n % 100) < 10) || ((n % 100) >= 20)) ? 1 : 2; } },

            // Romanian            
            PluralRule{ "nplurals=3;plural=(n==1?0:(((n%100>19)||((n%100==0)&&(n!=0)))?2:1));", [](onyxS32 n) { return (n == 1) ? 0 : (((n % 100) > 19) || (((n % 100) == 0) && (n != 0))) ? 2 : 1; } },

            // Slovenian
            PluralRule{ "nplurals=4;plural=(n%100==1?0:n%100==2?1:n%100==3||n%100==4?2:3);", [](onyxS32 n) { return ((n % 100) == 1) ? 0 : ((n % 100) == 2) ? 1 : ((n % 100) == 3) || ((n % 100) == 4) ? 2 : 3; } },

            // Slovak (alternative)
            PluralRule{ "nplurals=4;plural=(n%1==0&&n==1?0:n%1==0&&n>=2&&n<=4?1:n%1!=0?2:3);", [](onyxS32 n) { return ((n % 1) == 0) && (n == 1) ? 0 : ((n % 1) == 0) && (n >= 2) && (n <= 4) ? 1 : ((n % 1) != 0) ? 2 : 3; } },

            // Czech
            PluralRule{ "nplurals=4;plural=(n==1&&n%1==0)?0:(n>=2&&n<=4&&n%1==0)?1:(n%1!=0)?2:3;", [](onyxS32 n) { return (n == 1) && ((n % 1) == 0) ? 0 : ((n >= 2) && (n <= 4) && ((n % 1) == 0)) ? 1 : ((n % 1) != 0) ? 2 : 3; } },

            // Belarusian
            PluralRule{ "nplurals=4;plural=(n%10==1&&n%100!=11?0:n%10>=2&&n%10<=4&&(n%100<12||n%100>14)?1:n%10==0||(n%10>=5&&n%10<=9)||(n%100>=11&&n%100<=14)?2:3);", [](onyxS32 n) { return ((n % 10) == 1) && ((n % 100) != 11) ? 0 : ((n % 10) >= 2) && ((n % 10) <= 4) && (((n % 100) < 12) || ((n % 100) > 14)) ? 1 : ((n % 10) == 0) || (((n % 10) >= 5) && ((n % 10) <= 9)) || (((n % 100) >= 11) && ((n % 100) <= 14)) ? 2 : 3; } },

            // Scottish Gaelic
            PluralRule{ "nplurals=4;plural=(n==1||n==11)?0:(n==2||n==12)?1:(n>2&&n<20)?2:3;", [](onyxS32 n) { return (n == 1) || (n == 11) ? 0 : ((n == 2) || (n == 12)) ? 1 : ((n > 2) && (n < 20)) ? 2 : 3; } },

            // Welsh
            PluralRule{ "nplurals=4;plural=(n==1)?0:(n==2)?1:(n!=8&&n!=11)?2:3;", [](onyxS32 n) { return (n == 1) ? 0 : (n == 2) ? 1 : ((n != 8) && (n != 11)) ? 2 : 3; } },

            // Lithuanian (alternative)
            PluralRule{ "nplurals=4;plural=(n%10==1&&(n%100>19||n%100<11)?0:(n%10>=2&&n%10<=9)&&(n%100>19||n%100<11)?1:n%1!=0?2:3);", [](onyxS32 n) { return ((n % 10) == 1) && (((n % 100) > 19) || ((n % 100) < 11)) ? 0 : (((n % 10) >= 2) && ((n % 10) <= 9)) && (((n % 100) > 19) || ((n % 100) < 11)) ? 1 : ((n % 1) != 0) ? 2 : 3; } },

            // Ukranian
            PluralRule{ "nplurals=4;plural=(n%1==0&&n%10==1&&n%100!=11?0:n%1==0&&n%10>=2&&n%10<=4&&(n%100<12||n%100>14)?1:n%1==0&&(n%10==0||(n%10>=5&&n%10<=9)||(n%100>=11&&n%100<=14))?2:3);", [](onyxS32 n) { return ((n % 1) == 0) && ((n % 10) == 1) && ((n % 100) != 11) ? 0 : ((n % 1) == 0) && ((n % 10) >= 2) && ((n % 10) <= 4) && (((n % 100) < 12) || ((n % 100) > 14)) ? 1 : ((n % 1) == 0) && (((n % 10) == 0) || (((n % 10) >= 5) && ((n % 10) <= 9)) || (((n % 100) >= 11) && ((n % 100) <= 14))) ? 2 : 3; } },

            // Polish (alternative)
            PluralRule{ "nplurals=4;plural=(n==1?0:(n%10>=2&&n%10<=4)&&(n%100<12||n%100>14)?1:n!=1&&(n%10>=0&&n%10<=1)||(n%10>=5&&n%10<=9)||(n%100>=12&&n%100<=14)?2:3);", [](onyxS32 n) { return (n == 1) ? 0 : (((n % 10) >= 2) && ((n % 10) <= 4)) && (((n % 100) < 12) || ((n % 100) > 14)) ? 1 : (n != 1) && (((n % 10) >= 0) && ((n % 10) <= 1)) || (((n % 10) >= 5) && ((n % 10) <= 9)) || (((n % 100) >= 12) && ((n % 100) <= 14)) ? 2 : 3; } },

            // Hebrew
            PluralRule{ "nplurals=4;plural=(n==1&&n%1==0)?0:(n==2&&n%1==0)?1:(n%10==0&&n%1==0&&n>10)?2:3;", [](onyxS32 n) { return (n == 1) && ((n % 1) == 0) ? 0 : ((n == 2) && ((n % 1) == 0)) ? 1 : (((n % 10) == 0) && ((n % 1) == 0) && (n > 10)) ? 2 : 3; } },

            // Gaeilge Irish (alternative)
            PluralRule{ "nplurals=5;plural=(n==1?0:n==2?1:n<7?2:n<11?3:4)", [](onyxS32 n) { return (n == 1) ? 0 : (n == 2) ? 1 : (n < 7) ? 2 : (n < 11) ? 3 : 4; } },

            // Arabic
            PluralRule{ "nplurals=6;plural=n==0?0:n==1?1:n==2?2:n%100>=3&&n%100<=10?3:n%100>=11?4:5", [](onyxS32 n) { return ( n == 0 ) ? 0 : ( n == 1 ) ? 1 : ( n == 2 ) ? 2 : ( ( n % 100 ) >= 3 ) && ( ( n % 100 ) <= 10 ) ? 3 : ( ( n % 100 ) >= 11 ) ? 4 : 5; } }
        
        };

    }
    void GetTextLocalizationBackend::Init(Assets::AssetSystem& /*assetSystem*/, const LocalizationSettings& /*localizationSettings*/)
    {
        String fileContent;
        FileSystem::OnyxFile file("project:/localization/test.po");
        bool hasSucceeded = file.ReadAll(FileSystem::Path::GetFullPath("engine:/localization/test.po"), fileContent);

        if (hasSucceeded == false)
        {
            return;
        }

        StringStream stringStream(fileContent);

        // use english as default
        m_PluralFunction = PluralFunctions[1].Function;

        if (stringStream.ReadConditional(PO_HEADER_MSG_ID) &&
            stringStream.ReadConditional(PO_HEADER_MSG_STR))
        {
            StringView headerLine;
            while (stringStream.ReadLine(headerLine))
            {
                // reached end of header if an empty line is reached
                headerLine = Trim(headerLine);
                if (headerLine.empty())
                {
                    break;
                }

                // remove quotes and newline
                headerLine.remove_prefix(1);
                headerLine.remove_suffix(2);

                // implement parsing of plural form instead of that static lookup?
                if (IgnoreCaseStartsWith(headerLine, PO_HEADER_PLURAL_FORMS))
                {
                    headerLine.remove_prefix(PO_HEADER_PLURAL_FORMS.size());

                    if (headerLine.ends_with("\\n"))
                    {
                        headerLine.remove_suffix(2);
                    }

                    String pluralForm(headerLine);
                    std::erase_if(pluralForm, [](char c) { return std::isspace(c); });

                    auto it = std::ranges::find_if(PluralFunctions, [&](const PluralRule& rule) { return rule.Rule == pluralForm; });
                    if (it == PluralFunctions.end())
                    {
                        ONYX_LOG_WARNING("Failed finding language plural rule for {}", pluralForm);
                    }
                    else
                    {
                        m_PluralFunction = it->Function;
                    }
                }
            }
        }

        StringView localizationIdString;
        StringView localizationPluralIdString;
        StringView localizedText;
        while (stringStream.IsEof() == false)
        {
            if (stringStream.ReadConditional("msgid"))
            {
                stringStream.ReadString(localizationIdString);
            }

            if (stringStream.ReadConditional("msgid_plural"))
            {
                // for now we ignore it, not sure if we actually need the plural id
                stringStream.ReadString(localizationPluralIdString);
            }


            if ((localizationIdString.empty() == false) && stringStream.ReadConditional("msgstr"))
            {
                char peek;
                stringStream.Peek(peek);
                onyxS32 index = -1;
                if (peek == '[')
                {
                    stringStream.Skip(); // skip opening
                    stringStream.ReadType(index);
                    stringStream.Skip(); // skip closing
                }
                else
                {
                    index = 0;
                }

                stringStream.ReadString(localizedText);

                StringId32 id(localizationIdString);
                DynamicArray<String>& localizedTexts = m_LocaleDatabase[id];
                if (index >= static_cast<onyxS32>(localizedTexts.size()))
                {
                    localizedTexts.resize(index + 1);
                }
                else
                {
                    if (localizedTexts[index].empty() == false)
                    {
                        ONYX_LOG_WARNING("Duplicated entry for plural form found in localization file for id{} at plural index{}.", localizationIdString, index);
                    }
                }

                localizedTexts[index] = localizedText;
            }

            stringStream.SkipWhitespaces();
        }
    }

    Optional<StringView> GetTextLocalizationBackend::Localize(StringId32 id) const
    {
        auto it = m_LocaleDatabase.find(id);
        if (it == m_LocaleDatabase.end())
        {
            return std::nullopt;
        }

        return it->second.front();
    }

    Optional<StringView> GetTextLocalizationBackend::Localize(StringId32 id, onyxS32 count) const
    {
        auto it = m_LocaleDatabase.find(id);
        if (it == m_LocaleDatabase.end())
        {
            return std::nullopt;
        }

        onyxS32 index = m_PluralFunction(count);
        const DynamicArray<String>& localizedTexts = it->second;
        if (index > localizedTexts.size() )
        {
            return std::nullopt;
        }

        return localizedTexts[index];
    }
}
