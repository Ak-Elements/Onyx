#include <onyx/serialize/text/textserializer.h>

namespace onyx::serialization::text {

TextSerializer::TextSerializer()
    : m_root( 0, 0, 0, 0 ) {
    m_scopeStack.emplace( &m_root );
}

TextSerializer::TextSerializer( String&& data )
    : m_data( std::move( data ) )
    , m_root( 0, 0, 0, data.size() ) {
    m_scopeStack.emplace( &m_root );
    initializeScope( m_root );
}

TextSerializer::~TextSerializer() = default;

template < typename T >
bool TextSerializer::doGenericWrite( T value ) {
    Scope& currentScope = *m_scopeStack.top();

    [[maybe_unused]] StringView::size_type oldLength = currentScope.DataLength;

    StringView newData = format::format( "{}\n", value );
    [[maybe_unused]] const size_t newDataLength = newData.size();

    m_data.replace( currentScope.DataStart, currentScope.DataLength, newData );
    currentScope.DataLength = newDataLength;
    return true;
}

template < std::integral T >
bool TextSerializer::doGenericWrite( T value, uint8_t base ) {
    Scope& currentScope = *m_scopeStack.top();
    StringView newData;
    switch( base ) {
    case 2:
        newData = format::format( "{:b}\n", value );
        break;
    case 8:
        newData = format::format( "{:o}\n", value );
        break;
    case 10:
        newData = format::format( "{}\n", value );
        break;
    case 16:
        newData = format::format( "{:x}\n", value );
        break;
    default:
        ONYX_LOG_WARNING( "Unsupported base for json {}, falling back to base 10.", base );
        newData = format::format( "{}\n", value );
        break;
    }

    [[maybe_unused]] StringView::size_type oldLength = currentScope.DataLength;
    const StringView::size_type newDataLength = newData.size();

    m_data.replace( currentScope.DataStart, currentScope.DataLength, newData );
    currentScope.DataLength = newDataLength;
    return true;
}

template < typename T >
bool TextSerializer::doGenericWrite( StringView name, T value ) {
    if( createScope( name ) == false )
        return false;

    bool success = doGenericWrite( value );
    success &= endScope();
    return success;
}

template < std::integral T >
bool TextSerializer::doGenericWrite( StringView name, T value, uint8_t base ) {
    StringView valueAsBaseString;
    switch( base ) {
    case 2:
        valueAsBaseString = format::format( "{:b}\n", value );
        break;
    case 8:
        valueAsBaseString = format::format( "{:o}\n", value );
        break;
    case 10:
        valueAsBaseString = format::format( "{}\n", value );
        break;
    case 16:
        valueAsBaseString = format::format( "{:x}\n", value );
        break;
    default:
        ONYX_LOG_WARNING( "Unsupported base for json {}, falling back to base 10.", base );
        valueAsBaseString = format::format( "{}\n", value );
        break;
    }

    bool success = doGenericWrite( name, value );
    return success;
}

bool TextSerializer::doWrite( bool value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( StringView name, bool value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( int8_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( int16_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( int32_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( int64_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( uint8_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( uint16_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( uint32_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( uint64_t value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( float32 value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( float64 value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( int8_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( int16_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( int32_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( int64_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( uint8_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( uint16_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( uint32_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( uint64_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool TextSerializer::doWrite( StringView name, int8_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, int16_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, int32_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, int64_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, uint8_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, uint16_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, uint32_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, uint64_t value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, float32 value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, float64 value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::doWrite( StringView name, int8_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, int16_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, int32_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, int64_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, uint8_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, uint16_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, uint32_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView name, uint64_t value, uint8_t base ) {
    return doGenericWrite( name, value, base );
}

bool TextSerializer::doWrite( StringView value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( StringView name, StringView value ) {
    return doGenericWrite( name, value );
}

bool TextSerializer::createScope( uint32_t index ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() ) {
        currentScope.Children.resize( index + 1 );
        currentScope.Children[ index ].DataStart = currentScope.DataStart + currentScope.DataLength;
    }

    // intializescope if already present
    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    return true;
}

bool TextSerializer::createScope( uint64_t index ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() ) {
        currentScope.Children.resize( index + 1 );
        currentScope.Children[ index ].DataStart = currentScope.DataStart + currentScope.DataLength;
    }

    // intializescope if already present
    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    return true;
}

bool TextSerializer::createScope( StringView name ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    auto it = std::ranges::find_if( currentScope.Children, [ & ]( Scope& scope ) {
        return name == StringView( m_data ).substr( scope.NameStart, scope.NameLength );
    } );
    uint64_t index = std::distance( std::begin( currentScope.Children ), it );
    if( it == currentScope.Children.end() ) {
        auto currentScopeEnd = currentScope.DataStart + currentScope.DataLength;
        Scope newScope{ currentScopeEnd, name.size() };
        m_data += name;
        newScope.DataStart = currentScopeEnd + newScope.NameLength;
        newScope.DataLength = 0;
        currentScope.Children.emplace_back( newScope );
    }

    // intialize scope if already present
    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    return true;
}

bool TextSerializer::endScope() {
    ONYX_ASSERT( m_scopeStack.empty() == false );

    Scope& currentScope = *m_scopeStack.top();
    StringView data = StringView( m_data ).substr( currentScope.DataStart, currentScope.DataLength );
    auto scopeSize = std::ranges::count_if( data, []( char character ) { return character == '\n'; } );

    if( ( currentScope.NameLength != 0 ) && scopeSize == 1 ) {
        m_data.insert( currentScope.NameStart + currentScope.NameLength, 1, ':' );
        ++currentScope.DataStart;
    } else if( currentScope.NameLength != 0 ) {
        const auto scopeSizeStart = currentScope.NameStart + currentScope.NameLength;
        const auto scopeSizeLength = currentScope.DataStart - scopeSizeStart;

        StringView newScopeSize = format::format( " {}\n", scopeSize );
        m_data.replace( scopeSizeStart, scopeSizeLength, newScopeSize );
        const auto deltaSize = numericCast< int64_t >( newScopeSize.size() ) -
                               numericCast< int64_t >( scopeSizeLength );
        currentScope.DataStart += deltaSize;

        for( Scope& child : currentScope.Children ) {
            updateScopeRange( child, deltaSize );
        }
    }

    m_scopeStack.pop();

    // adjust parent scope length
    Scope& parentScope = *m_scopeStack.top();
    parentScope.DataLength = ( currentScope.DataStart + currentScope.DataLength ) - parentScope.DataStart;
    if( m_scopeStack.size() == 1 ) {
        m_data.resize( parentScope.DataLength );
    }
    return true;
}

void TextSerializer::initializeScope( Scope& scope ) {
    const StringView currentData = StringView( m_data ).substr( scope.DataStart, scope.DataLength );

    const StringView::size_type scopeLength = currentData.size();
    const StringView::size_type scopeEndIndex = scopeLength - 1;

    for( StringView::size_type i = 0; i < scopeLength; ++i ) {
        if( currentData[ i ] == '/' && currentData[ std::min( i + 1, scopeEndIndex ) ] == '/' ) {
            const auto newlineIndex = currentData.find_first_of( '\n', i );
            i = ( newlineIndex == StringView::npos ) ? scopeEndIndex : newlineIndex;
            continue;
        }

        const uint64_t splitIndex = std::min( currentData.find_first_of( ":\n", i ), scopeEndIndex );

        // simple property
        if( currentData[ splitIndex ] == ':' ) {
            // TODO: this needs to be smarter to ignore \n in strings
            const uint64_t valueStartIndex = currentData.find_first_not_of( " \t\n\r", splitIndex + 1 );

            const char* endCharacters = "\t\n\r";
            if( currentData[ valueStartIndex ] == '"' )
                endCharacters = "\"";
            else if( currentData[ valueStartIndex ] == '\'' )
                endCharacters = "'";

            const uint64_t valueEndIndex = std::min( currentData.find_first_of( endCharacters, valueStartIndex + 1 ),
                                                     scopeEndIndex );
            scope.Children.emplace_back( i, splitIndex - i, valueStartIndex, valueEndIndex - valueStartIndex );
            i = valueEndIndex;
        } else {
            StringView trimmedLine = trimLeft( currentData.substr( i, splitIndex - i ) );
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

            uint64_t scopeLineCount = 1;
            if( propertyNameEndIndex != splitIndex ) {
                StringView lineCountString = trim( trimmedLine.substr( propertyNameEndIndex ) );
                if( lineCountString.empty() == false ) {
                    std::errc error = toNumeric( lineCountString, scopeLineCount );
                    if( error != std::errc() ) {
                        ONYX_LOG_WARNING( "Failed to parse scope count for {}", propertyName );
                    }
                }
            }

            // advance past the header line before counting child lines
            const uint64_t scopeStartIndex = splitIndex + 1;

            // TODO: handle linebreaks in strings
            for( uint64_t j = 0; j < scopeLineCount; ++j ) {
                const uint64_t endOfLineIndex = std::min( currentData.find_first_of( '\n', i + 1 ), scopeEndIndex );
                i = endOfLineIndex;
            }

            // auto propertyData = currentData.substr( scopeStartIndex, i - scopeStartIndex );
            // if( propertyData.empty() ) {
            //     propertyData = propertyName;
            // }
            scope.Children.emplace_back( 0, propertyNameEndIndex, scopeStartIndex, i - scopeStartIndex );
        }
    }
}

void TextSerializer::updateScopeRange( Scope& scope, int64_t delta ) {
    scope.NameStart += delta;
    scope.DataStart += delta;
    for( Scope& child : scope.Children ) {
        updateScopeRange( child, delta );
    }
}
} // namespace onyx::serialization::text
