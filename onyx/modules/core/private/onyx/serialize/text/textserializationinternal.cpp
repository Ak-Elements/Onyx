#include <onyx/serialize/text/textserializationinternal.h>

namespace onyx::serialization::text::internal {

namespace {
inline StringView::size_type findUnescapedChar( StringView data, char quoteChar, StringView::size_type from ) {
    for( StringView::size_type i = from; i < data.size(); ++i ) {
        if( data[ i ] == '\\' ) {
            ++i; // unconditionally skip whatever follows the backslash
            continue;
        }
        if( data[ i ] == quoteChar ) {
            return i;
        }
    }
    return StringView::npos;
}
} // namespace

void initializeScope( StringView content, Scope& scope ) {
    // const StringView currentData = content.substr( scope.DataStart, scope.DataLength );

    const StringView::size_type scopeLength = scope.DataLength;
    const StringView::size_type scopeEndIndex = scope.DataStart + scopeLength - 1;

    for( StringView::size_type i = scope.DataStart; i <= scopeEndIndex; ++i ) {
        // const StringView currentData = content.substr( scope.DataStart, scope.DataLength );
        [[maybe_unused]] StringView firstCharacter = content.substr( i, 2 );
        if( content[ i ] == '/' && content[ std::min( i + 1, scopeEndIndex ) ] == '/' ) {
            const auto newlineIndex = content.find_first_of( '\n', i );
            i = ( newlineIndex == StringView::npos ) ? scopeEndIndex : newlineIndex;
            continue;
        }

        const uint64_t splitIndex = std::min( content.find_first_of( ":\n", i ), scopeEndIndex );

        // simple property
        if( content[ splitIndex ] == ':' ) {
            // TODO: this needs to be smarter to ignore \n in strings
            const uint64_t propertyNameStartIndex = content.find_first_not_of( " \t", i );
            const uint64_t propertyNameEndIndex = splitIndex;

            const uint64_t valueStartIndex = content.find_first_not_of( " \t\n\r", splitIndex + 1 );
            uint64_t valueEndIndex = valueStartIndex;

            bool isQuoted = false;
            if( content[ valueStartIndex ] == '"' ) {
                isQuoted = true;
                valueEndIndex = std::min( findUnescapedChar( content, '"', valueStartIndex + 1 ), scopeEndIndex );
            } else if( content[ valueStartIndex ] == '\'' ) {
                isQuoted = true;
                valueEndIndex = std::min( findUnescapedChar( content, '\'', valueStartIndex + 1 ), scopeEndIndex );
            } else {
                valueEndIndex = std::min( content.find_first_of( "\t\n\r", valueStartIndex + 1 ), scopeEndIndex );
            }

            if( isQuoted == false ) {
                const StringView rawSpan = content.substr( valueStartIndex, valueEndIndex - valueStartIndex );
                const StringView::size_type commentIndex = rawSpan.find( "//" );
                if( commentIndex != StringView::npos ) {
                    valueEndIndex = valueStartIndex + commentIndex;
                    const StringView trimmedValue = trimRight(
                        content.substr( valueStartIndex, valueEndIndex - valueStartIndex ) );
                    valueEndIndex = valueStartIndex + trimmedValue.size();
                }
            }

            [[maybe_unused]] StringView name = content.substr( propertyNameStartIndex,
                                                               propertyNameEndIndex - propertyNameStartIndex );
            [[maybe_unused]] StringView childContent = content.substr( valueStartIndex,
                                                                       valueEndIndex - valueStartIndex );

            scope.Children.emplace_back( propertyNameStartIndex,
                                         propertyNameEndIndex - propertyNameStartIndex,
                                         valueStartIndex,
                                         valueEndIndex - valueStartIndex + 1,
                                         true );

            StringView::size_type endIndex = content.find_first_not_of( " \t\n\r", valueEndIndex + 1 );
            if( endIndex != StringView::npos ) {
                i = endIndex - 1;
            } else {
                i = valueEndIndex;
            }
        } else {
            StringView::size_type propertyNameStartIndex = content.find_first_not_of( " \n\r\t", i );
            StringView trimmedLine = content.substr( propertyNameStartIndex, splitIndex - propertyNameStartIndex );
            if( trimmedLine.empty() ) {
                i = splitIndex;
                continue;
            }

            // TODO: We probably need to handle strings here properly
            uint64_t commentOrStringStartIndex = trimmedLine.find_first_of( "\"'/" );
            if( commentOrStringStartIndex != StringView::npos ) {
                if( trimmedLine[ commentOrStringStartIndex ] == '/' &&
                    trimmedLine[ std::min( commentOrStringStartIndex + 1, scopeEndIndex ) ] == '/' ) {
                    trimmedLine = trimRight( trimmedLine.substr( 0, commentOrStringStartIndex ) );
                }
            }

            uint64_t propertyNameEndIndex = trimmedLine.find_first_of( " \t\n\r" );
            if( propertyNameEndIndex == StringView::npos ) {
                propertyNameEndIndex = splitIndex;
            }

            StringView propertyName = trim( trimmedLine.substr( 0, propertyNameEndIndex ) );

            uint64_t scopeLineCount = 0;
            if( propertyNameEndIndex != splitIndex ) {
                StringView lineCountString = trim( trimmedLine.substr( propertyNameEndIndex ) );
                if( lineCountString.empty() == false ) {
                    std::errc error = toNumeric( lineCountString, scopeLineCount );
                    if( error != std::errc() ) {
                        ONYX_LOG_WARNING( "Failed to parse scope count for {}", propertyName );
                    }
                }
            }
            //

            if( scopeLineCount == 0 ) {
                scope.Children.emplace_back( propertyNameStartIndex, propertyNameEndIndex, propertyNameEndIndex, 0 );
                i = splitIndex;
                continue;
            }
            //
            // advance past the header line before counting child lines
            const uint64_t scopeStartIndex = splitIndex + 1;

            // TODO: handle linebreaks in strings
            uint64_t searchPos = scopeStartIndex;
            for( uint64_t j = 0; j < scopeLineCount; ++j ) {
                const uint64_t endOfLineIndex = std::min( content.find_first_of( '\n', searchPos ), scopeEndIndex );
                i = endOfLineIndex;
                searchPos = endOfLineIndex + 1;
            }

            auto propertyData = content.substr( scopeStartIndex, i - scopeStartIndex + 1 );
            if( propertyData.empty() ) {
                propertyData = propertyName;
            }

            [[maybe_unused]] StringView name = content.substr( propertyNameStartIndex, propertyNameEndIndex );
            [[maybe_unused]] StringView childContent = content.substr( scopeStartIndex,

                                                                       i - scopeStartIndex + 1 );
            scope.Children.emplace_back( propertyNameStartIndex,
                                         propertyNameEndIndex,
                                         scopeStartIndex,
                                         i - scopeStartIndex + 1,
                                         false );
        }
    }
}

void updateScopeRange( Scope& scope, int64_t delta ) {
    scope.NameStart += delta;
    scope.DataStart += delta;
    for( Scope& child : scope.Children ) {
        updateScopeRange( child, delta );
    }
}

} // namespace onyx::serialization::text::internal
