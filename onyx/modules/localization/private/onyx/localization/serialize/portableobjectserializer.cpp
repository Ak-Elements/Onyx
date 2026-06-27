#include <onyx/localization/serialize/portableobjectserializer.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/localization/assets/gettextlocalizationdatabase.h>
#include <onyx/stream/stringstream.h>

namespace onyx::localization {
namespace {
constexpr StringView PO_HEADER_MSG_ID = "msgid \"\"\n";
constexpr StringView PO_HEADER_MSG_STR = "msgstr \"\"\n";
constexpr StringView PO_HEADER_PLURAL_FORMS = "plural-forms:";

struct PluralRule {
    using PluralFunc = int32_t ( * )( int32_t );

    StringView Rule;
    PluralFunc Function;
};

constexpr Array< PluralRule, 24 > PluralFunctions = {
    // Note that the plural forms here shouldn't contain any spaces
    // Japanese, Vietnamese, Korean
    // Thai
    PluralRule{ "nplurals=1;plural=0;", []( int32_t ) { return 0; } },

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
    PluralRule{ "nplurals=2;plural=(n!=1);", []( int32_t n ) { return n != 1 ? 1 : 0; } },

    /// Brazilian Portuguese, French
    PluralRule{ "nplurals=2;plural=(n>1);", []( int32_t n ) { return n > 1 ? 1 : 0; } },

    // Macedonian
    PluralRule{ "nplurals=2;plural=n==1||n%10==1?0:1;",
                []( int32_t n ) { return ( n == 1 ) || ( ( n % 10 ) == 1 ) ? 0 : 1; } },

    // Macedonian 2
    PluralRule{ "nplurals=2;plural=(n%10==1&&n%100!=11)?0:1;",
                []( int32_t n ) { return ( ( n % 10 ) == 1 ) && ( ( n % 100 ) != 11 ) ? 0 : 1; } },

    // Latvian
    PluralRule{ "nplurals=3;plural=n%10==1&&n%100!=11?0:n!=0?1:2);",
                []( int32_t n ) { return ( ( n % 10 ) == 1 ) && ( ( n % 100 ) != 11 ) ? 0
                                         : ( n != 0 )                                 ? 1
                                                                                      : 2; } },

    // Gaeilge Irish
    PluralRule{ "nplurals=3;plural=n==1?0:n==2?1:2;", []( int32_t n ) { return ( n == 1 )   ? 0
                                                                               : ( n == 2 ) ? 1
                                                                                            : 2; } },

    // Lithuanian
    PluralRule{ "nplurals=3;plural=(n%10==1&&n%100!=11?0:n%10>=2&&(n%100<10||n%100>=20)?1:2);",
                []( int32_t n ) {
                    return ( ( n % 10 ) == 1 ) && ( ( n % 100 ) != 11 )                               ? 0
                           : ( ( n % 10 ) >= 2 ) && ( ( ( n % 100 ) < 10 ) || ( ( n % 100 ) >= 20 ) ) ? 1
                                                                                                      : 2;
                } },

    // Russian, Ukrainian, Belarusian, Serbian, Croatian
    PluralRule{ "nplurals=3;plural=(n%10==1&&n%100!=11?0:n%10>=2&&n%10<=4&&(n%100<10||n%100>=20)?1:2);",
                []( int32_t n ) {
                    return ( ( n % 10 ) == 1 ) && ( ( n % 100 ) != 11 ) ? 0
                           : ( ( n % 10 ) >= 2 ) && ( ( n % 10 ) <= 4 ) &&
                                   ( ( ( n % 100 ) < 10 ) || ( ( n % 100 ) >= 20 ) )
                               ? 1
                               : 2;
                } },

    // Czech, Slovak
    PluralRule{ "nplurals=3;plural=(n==1)?0:(n>=2&&n<=4)?1:2;",
                []( int32_t n ) { return ( n == 1 )                 ? 0
                                         : ( n >= 2 ) && ( n <= 4 ) ? 1
                                                                    : 2; } },

    // Polish
    PluralRule{ "nplurals=3;plural=(n==1?0:n%10>=2&&n%10<=4&&(n%100<10||n%100>=20)?1:2);",
                []( int32_t n ) {
                    return ( n == 1 ) ? 0
                           : ( ( n % 10 ) >= 2 ) && ( ( n % 10 ) <= 4 ) &&
                                   ( ( ( n % 100 ) < 10 ) || ( ( n % 100 ) >= 20 ) )
                               ? 1
                               : 2;
                } },

    // Romanian
    PluralRule{ "nplurals=3;plural=(n==1?0:(((n%100>19)||((n%100==0)&&(n!=0)))?2:1));",
                []( int32_t n ) {
                    return ( n == 1 ) ? 0 : ( ( ( n % 100 ) > 19 ) || ( ( ( n % 100 ) == 0 ) && ( n != 0 ) ) ) ? 2 : 1;
                } },

    // Slovenian
    PluralRule{ "nplurals=4;plural=(n%100==1?0:n%100==2?1:n%100==3||n%100==4?2:3);",
                []( int32_t n ) {
                    return ( ( n % 100 ) == 1 )                           ? 0
                           : ( ( n % 100 ) == 2 )                         ? 1
                           : ( ( n % 100 ) == 3 ) || ( ( n % 100 ) == 4 ) ? 2
                                                                          : 3;
                } },

    // Slovak (alternative)
    PluralRule{ "nplurals=4;plural=(n%1==0&&n==1?0:n%1==0&&n>=2&&n<=4?1:n%1!=0?2:3);",
                []( int32_t n ) {
                    return ( ( n % 1 ) == 0 ) && ( n == 1 )                 ? 0
                           : ( ( n % 1 ) == 0 ) && ( n >= 2 ) && ( n <= 4 ) ? 1
                           : ( ( n % 1 ) != 0 )                             ? 2
                                                                            : 3;
                } },

    // Czech
    PluralRule{ "nplurals=4;plural=(n==1&&n%1==0)?0:(n>=2&&n<=4&&n%1==0)?1:(n%1!=0)?2:3;",
                []( int32_t n ) {
                    return ( n == 1 ) && ( ( n % 1 ) == 0 )                     ? 0
                           : ( ( n >= 2 ) && ( n <= 4 ) && ( ( n % 1 ) == 0 ) ) ? 1
                           : ( ( n % 1 ) != 0 )                                 ? 2
                                                                                : 3;
                } },

    // Belarusian
    PluralRule{ "nplurals=4;plural=(n%10==1&&n%100!=11?0:n%10>=2&&n%10<=4&&(n%100<12||n%100>14)?1:n%10==0||(n%10>=5&&n%"
                "10<=9)||(n%100>=11&&n%100<=14)?2:3);",
                []( int32_t n ) {
                    return ( ( n % 10 ) == 1 ) && ( ( n % 100 ) != 11 ) ? 0
                           : ( ( n % 10 ) >= 2 ) && ( ( n % 10 ) <= 4 ) &&
                                   ( ( ( n % 100 ) < 12 ) || ( ( n % 100 ) > 14 ) )
                               ? 1
                           : ( ( n % 10 ) == 0 ) || ( ( ( n % 10 ) >= 5 ) && ( ( n % 10 ) <= 9 ) ) ||
                                   ( ( ( n % 100 ) >= 11 ) && ( ( n % 100 ) <= 14 ) )
                               ? 2
                               : 3;
                } },

    // Scottish Gaelic
    PluralRule{ "nplurals=4;plural=(n==1||n==11)?0:(n==2||n==12)?1:(n>2&&n<20)?2:3;",
                []( int32_t n ) {
                    return ( n == 1 ) || ( n == 11 )       ? 0
                           : ( ( n == 2 ) || ( n == 12 ) ) ? 1
                           : ( ( n > 2 ) && ( n < 20 ) )   ? 2
                                                           : 3;
                } },

    // Welsh
    PluralRule{ "nplurals=4;plural=(n==1)?0:(n==2)?1:(n!=8&&n!=11)?2:3;",
                []( int32_t n ) {
                    return ( n == 1 ) ? 0 : ( n == 2 ) ? 1 : ( ( n != 8 ) && ( n != 11 ) ) ? 2 : 3;
                } },

    // Lithuanian (alternative)
    PluralRule{
        "nplurals=4;plural=(n%10==1&&(n%100>19||n%100<11)?0:(n%10>=2&&n%10<=9)&&(n%100>19||n%100<11)?1:n%1!=0?2:3);",
        []( int32_t n ) {
            return ( ( n % 10 ) == 1 ) && ( ( ( n % 100 ) > 19 ) || ( ( n % 100 ) < 11 ) ) ? 0
                   : ( ( ( n % 10 ) >= 2 ) && ( ( n % 10 ) <= 9 ) ) && ( ( ( n % 100 ) > 19 ) || ( ( n % 100 ) < 11 ) )
                       ? 1
                   : ( ( n % 1 ) != 0 ) ? 2
                                        : 3;
        } },

    // Ukranian
    PluralRule{ "nplurals=4;plural=(n%1==0&&n%10==1&&n%100!=11?0:n%1==0&&n%10>=2&&n%10<=4&&(n%100<12||n%100>14)?1:n%1=="
                "0&&(n%10==0||(n%10>=5&&n%10<=9)||(n%100>=11&&n%100<=14))?2:3);",
                []( int32_t n ) {
                    return ( ( n % 1 ) == 0 ) && ( ( n % 10 ) == 1 ) && ( ( n % 100 ) != 11 ) ? 0
                           : ( ( n % 1 ) == 0 ) && ( ( n % 10 ) >= 2 ) && ( ( n % 10 ) <= 4 ) &&
                                   ( ( ( n % 100 ) < 12 ) || ( ( n % 100 ) > 14 ) )
                               ? 1
                           : ( ( n % 1 ) == 0 ) &&
                                   ( ( ( n % 10 ) == 0 ) || ( ( ( n % 10 ) >= 5 ) && ( ( n % 10 ) <= 9 ) ) ||
                                     ( ( ( n % 100 ) >= 11 ) && ( ( n % 100 ) <= 14 ) ) )
                               ? 2
                               : 3;
                } },

    // Polish (alternative)
    PluralRule{ "nplurals=4;plural=(n==1?0:(n%10>=2&&n%10<=4)&&(n%100<12||n%100>14)?1:n!=1&&(n%10>=0&&n%10<=1)||(n%10>="
                "5&&n%10<=9)||(n%100>=12&&n%100<=14)?2:3);",
                []( int32_t n ) {
                    return ( n == 1 ) ? 0
                           : ( ( ( n % 10 ) >= 2 ) && ( ( n % 10 ) <= 4 ) ) &&
                                   ( ( ( n % 100 ) < 12 ) || ( ( n % 100 ) > 14 ) )
                               ? 1
                           : ( ( n != 1 ) && ( ( n % 10 ) >= 0 ) && ( ( n % 10 ) <= 1 ) ) ||
                                   ( ( ( n % 10 ) >= 5 ) && ( ( n % 10 ) <= 9 ) ) ||
                                   ( ( ( n % 100 ) >= 12 ) && ( ( n % 100 ) <= 14 ) )
                               ? 2
                               : 3;
                } },

    // Hebrew
    PluralRule{ "nplurals=4;plural=(n==1&&n%1==0)?0:(n==2&&n%1==0)?1:(n%10==0&&n%1==0&&n>10)?2:3;",
                []( int32_t n ) {
                    return ( n == 1 ) && ( ( n % 1 ) == 0 )                              ? 0
                           : ( ( n == 2 ) && ( ( n % 1 ) == 0 ) )                        ? 1
                           : ( ( ( n % 10 ) == 0 ) && ( ( n % 1 ) == 0 ) && ( n > 10 ) ) ? 2
                                                                                         : 3;
                } },

    // Gaeilge Irish (alternative)
    PluralRule{ "nplurals=5;plural=(n==1?0:n==2?1:n<7?2:n<11?3:4)",
                []( int32_t n ) {
                    return ( n == 1 ) ? 0 : ( n == 2 ) ? 1 : ( n < 7 ) ? 2 : ( n < 11 ) ? 3 : 4;
                } },

    // Arabic
    PluralRule{ "nplurals=6;plural=n==0?0:n==1?1:n==2?2:n%100>=3&&n%100<=10?3:n%100>=11?4:5", []( int32_t n ) {
                   return ( n == 0 )                                      ? 0
                          : ( n == 1 )                                    ? 1
                          : ( n == 2 )                                    ? 2
                          : ( ( n % 100 ) >= 3 ) && ( ( n % 100 ) <= 10 ) ? 3
                          : ( ( n % 100 ) >= 11 )                         ? 4
                                                                          : 5;
               } } };

bool ParsePoFile( const FilePath& path, GetTextLocalizationDatabase& outLocalizationMap ) {
    String fileContent;
    bool hasSucceeded = file_system::OnyxFile::ReadAll( path, fileContent );

    if( hasSucceeded == false ) {
        return false;
    }

    StringStream stringStream( fileContent );
    if( stringStream.isEof() )
        return true;

    // use english as default
    auto pluralFunction = PluralFunctions[ 1 ].Function;

    if( stringStream.readConditional( PO_HEADER_MSG_ID ) && stringStream.readConditional( PO_HEADER_MSG_STR ) ) {
        StringView headerLine;
        while( stringStream.isEof() == false ) {
            stringStream.readLine( headerLine );
            // reached end of header if an empty line is reached
            headerLine = trim( headerLine );
            if( headerLine.empty() ) {
                break;
            }

            // remove quotes
            headerLine.remove_prefix( 1 );
            headerLine.remove_suffix( 1 );

            // implement parsing of plural form instead of that static lookup?
            if( ignoreCaseStartsWith( headerLine, PO_HEADER_PLURAL_FORMS ) ) {
                headerLine.remove_prefix( PO_HEADER_PLURAL_FORMS.size() );

                String pluralForm( headerLine );
                std::erase_if( pluralForm, []( char c ) { return std::isspace( c ); } );

                auto it = std::ranges::find_if( PluralFunctions,
                                                [ & ]( const PluralRule& rule ) { return rule.Rule == pluralForm; } );
                if( ( pluralFunction == nullptr ) && ( it == PluralFunctions.end() ) ) {
                    ONYX_LOG_WARNING( "Failed finding language plural rule for {}", pluralForm );
                } else {
                    pluralFunction = it->Function;
                }
            }
        }
    }

    outLocalizationMap.setPluralFunction( pluralFunction );

    HashMap< LocalizationId, DynamicArray< String > >& localeDatabase = outLocalizationMap.getDatabase();
    localeDatabase.clear();

    StringView localizationIdString;
    StringView localizationPluralIdString;
    StringView localizedText;
    StringView contextIdString;
    char peek;

    LocalizationId localizationId;
    while( stringStream.isEof() == false ) {
        stringStream.peek( peek );
        if( peek == '#' ) {
            StringView line;
            stringStream.readLine( line );
            continue;
        }

        if( stringStream.readConditional( "msgid_plural" ) ) {
            // for now we ignore it, not sure if we actually need the plural id
            stringStream.readString( localizationPluralIdString );
        } else if( stringStream.readConditional( "msgid" ) ) {
            stringStream.readString( localizationIdString );
            if( localizationId.Id.isValid() ) {
                localizationId.Context.reset();
            }

            localizationId.Id = localizationIdString;
        }

        if( stringStream.readConditional( "msgctx" ) ) {
            stringStream.readString( contextIdString );
            localizationId.Context = contextIdString;
            localizationId.Id.reset();
        }

        if( ( localizationId.Id.isValid() ) && stringStream.readConditional( "msgstr" ) ) {
            stringStream.peek( peek );
            int32_t index = -1;
            if( peek == '[' ) {
                stringStream.skip(); // skip opening
                stringStream.readType( index );
                stringStream.skip(); // skip closing
            } else {
                index = 0;
            }

            stringStream.readString( localizedText );

            DynamicArray< String >& localizedTexts = localeDatabase[ localizationId ];
            if( index >= static_cast< int32_t >( localizedTexts.size() ) ) {
                localizedTexts.resize( index + 1 );
            } else {
                if( localizedTexts[ index ].empty() == false ) {
                    ONYX_LOG_WARNING(
                        "Duplicated entry for plural form found in localization file for id{} at plural index{}.",
                        localizationIdString,
                        index );
                }
            }

            localizedTexts[ index ] = localizedText;
        }

        stringStream.skipWhitespaces();
    }

    return true;
}
} // namespace

bool PortableObjectSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& /*asset*/,
                                          const assets::AssetMetaData& /*meta*/,
                                          Serializer& /*serializer*/,
                                          const IEngine& /*engine*/ ) const {
    // we can't save po files as they are created in a localization editor
    return false;
}

bool PortableObjectSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                            const assets::AssetMetaData& meta,
                                            const Deserializer& /*deserializer*/,
                                            IEngine& /*engine*/ ) const {
    // po files are not json or yaml so we do not use the provided serializer and instead read the file as raw text
    GetTextLocalizationDatabase& localizationDatabase = asset.as< GetTextLocalizationDatabase >();
    return ParsePoFile( file_system::path::getFullPath( meta.Path ), localizationDatabase );
}
} // namespace onyx::localization
