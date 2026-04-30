#include <onyx/filesystem/textdeserializer.h>

namespace onyx::file_system {
TextDeserializer::TextDeserializer( StringView data )
    : m_root( "root", data ) {
    m_scopeStack.emplace( &m_root );
    updateScope();
}

template < typename T >
bool TextDeserializer::doGenericRead( T& outValue ) const {
    const Scope& currentScope = *m_scopeStack.top();
    const StringView currentData = currentScope.Data;

    StringView::size_type lastIndex = StringView::npos;
    if constexpr( std::is_same_v< bool, T > ) {
        lastIndex = currentData.find_first_of( ' ' );
        StringView value = currentData.substr( 0, lastIndex );
        outValue = ( value == "1" ) || ignoreCaseEqual( value, "true" );
    } else if constexpr( std::is_arithmetic_v< T > ) {
        StringView::size_type startIndex = currentData.find_first_not_of( " \t\n\r" );
        lastIndex = currentData.find_first_of( " \t\n\r", startIndex + 1 );
        StringView value = currentData.substr( startIndex, lastIndex - startIndex );

        uint8_t base = 10;
        if( value[ 0 ] == '#' ) {
            value.remove_prefix( 1 );
            base = 16;
        }

        bool success = toNumeric( value, outValue, base ) == std::errc();
        if( success == false ) {
            return false;
        }
    } else if constexpr( std::is_same_v< StringView, T > ) {
        const char* endCharacters = "\t\n\r";
        if( currentData[ 0 ] == '"' )
            endCharacters = "\"";
        else if( currentData[ 0 ] == '\'' )
            endCharacters = "'";

        StringView::size_type startIndex = currentData.find_first_not_of( endCharacters );
        lastIndex = currentData.find_first_of( endCharacters, ( startIndex + 1 ) );
        outValue = currentData.substr( startIndex, ( lastIndex - startIndex ) );
    }

    // m_index = m_data.find_first_not_of( " \t\n\r", lastIndex + 1 );
    return true;
}

template < typename T >
bool TextDeserializer::doGenericRead( StringView name, T& outValue ) const {
    if( createScope( name ) == false )
        return false;

    bool success = doGenericRead( outValue );
    success &= endScope();
    return success;
}

template < Numeric T >
bool TextDeserializer::doGenericRead( T& outValue, uint8_t base ) const {
    const Scope& currentScope = *m_scopeStack.top();
    const StringView currentData = currentScope.Data;

    StringView::size_type lastIndex = currentData.find_first_of( " \t\n\r" );
    StringView value = currentData.substr( 0, lastIndex );

    if( value[ 0 ] == '#' )
        value.remove_prefix( 1 );

    bool success = toNumeric( value, outValue, base ) == std::errc();
    // m_index += m_data.find_first_not_of( " \t\n\r", lastIndex + 1 ) - m_index;
    return success;
}

template < Numeric T >
bool TextDeserializer::doGenericRead( StringView name, T& outValue, uint8_t base ) const {
    // auto it = m_scopes.find( StringId32( name ) );
    // if( it == m_scopes.end() )
    //     return false;
    //
    // m_index = it->second.StartIndex;
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( bool& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( StringView name, bool& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( int8_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( int16_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( int32_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( int64_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( uint8_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( uint16_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( uint32_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( uint64_t& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( float32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( float32& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( float64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( float64& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( int8_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( int16_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( int32_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( int64_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( uint8_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( uint16_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( uint32_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( uint64_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::doRead( StringView name, int8_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, int16_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, int32_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, int64_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, uint8_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, uint16_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, uint32_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, uint64_t& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, float32& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, float32& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, float64& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::doRead( StringView name, float64& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, int8_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, int16_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, int32_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, int64_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, uint8_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, uint16_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, uint32_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView name, uint64_t& outValue, uint8_t base ) const {
    return doGenericRead( name, outValue, base );
}

bool TextDeserializer::doRead( StringView& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( StringView name, StringView& outValue ) const {
    return doGenericRead( name, outValue );
}

bool TextDeserializer::createScope( uint32_t index ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() )
        return false;

    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    updateScope();
    return true;
}

bool TextDeserializer::createScope( uint64_t index ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() )
        return false;

    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    updateScope();
    return true;
}

bool TextDeserializer::createScope( StringView name ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    auto it = std::ranges::find_if( currentScope.Children, [ name ]( Scope& scope ) { return name == scope.Name; } );
    if( it == currentScope.Children.end() )
        return false;

    uint64_t index = std::distance( std::begin( currentScope.Children ), it );
    return createScope( index );
}

bool TextDeserializer::getScopeIdentifier( uint32_t& /*outKey*/ ) const {
    ONYX_ASSERT( false, "Integral scope identifiers not supported in json" );
    return false;
}

bool TextDeserializer::getScopeIdentifier( uint64_t& /*outKey*/ ) const {
    ONYX_ASSERT( false, "Integral scope identifiers not supported in json" );
    return false;
}

bool TextDeserializer::getScopeIdentifier( Guid64& outKey ) const {
    return false;
}

bool TextDeserializer::getScopeIdentifier( StringView& outKey ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    const Scope& currentScope = *m_scopeStack.top();
    outKey = currentScope.Name;
    return true;
}

bool TextDeserializer::endScope() const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    m_scopeStack.pop();
    return true;
}

uint32_t TextDeserializer::getItemsCount() const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    const Scope& currentScope = *m_scopeStack.top();
    return numericCast< uint32_t >( currentScope.Children.size() );
}

void TextDeserializer::updateScope() const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    const StringView currentData = currentScope.Data;

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
            StringView propertyName = trim( currentData.substr( i, splitIndex - i ) );
            // TODO: this needs to be smarter to ignore \n in strings
            const uint64_t valueStartIndex = currentData.find_first_not_of( " \t\n\r", splitIndex + 1 );

            const char* endCharacters = "\t\n\r";
            if( currentData[ valueStartIndex ] == '"' )
                endCharacters = "\"";
            else if( currentData[ valueStartIndex ] == '\'' )
                endCharacters = "'";

            const uint64_t valueEndIndex = std::min( currentData.find_first_of( endCharacters, valueStartIndex + 1 ),
                                                     scopeEndIndex );
            currentScope.Children.emplace_back(
                propertyName,
                currentData.substr( valueStartIndex, valueEndIndex - valueStartIndex ) );

            i = valueEndIndex;
        } else {
            StringView trimmedLine = trimLeft( currentData.substr( i, splitIndex - i ) );
            if( trimmedLine.empty() ) {
                i = splitIndex;
                continue;
            }

            uint64_t propertyNameEndIndex = trimmedLine.find_first_of( " \t\n\r" );
            if( propertyNameEndIndex == StringView::npos ) {
                propertyNameEndIndex = splitIndex;
            }
            StringView propertyName = trim( trimmedLine.substr( 0, propertyNameEndIndex ) );
            if( propertyName.empty() ) {
                ONYX_LOG_WARNING( "t" );
            }
            if( propertyName == "colors" ) {
                ONYX_LOG_WARNING( "t" );
            }

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

            auto propertyData = currentData.substr( scopeStartIndex, i - scopeStartIndex );
            if( propertyData.empty() ) {
                propertyData = propertyName;
            }
            currentScope.Children.emplace_back( propertyName, propertyData );
        }
    }
}

} // namespace onyx::file_system
