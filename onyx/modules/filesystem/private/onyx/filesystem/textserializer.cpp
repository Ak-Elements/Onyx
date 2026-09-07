#include <onyx/filesystem/textserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system {

TextSerializer::TextSerializer() {
    m_scopeStack.emplace( &m_root );
}

String TextSerializer::toString() const {
    auto scopeToString = []( this auto& self, const Scope& scope ) -> String {
        String content;
        for( const Scope& child : scope.Children ) {
            content += self( child );
        }

        uint64_t length = std::ranges::count_if( content, []( char c ) { return c == '\n'; } );

        if( length == 0 )
            return format::format( "{}:{}\n", scope.Name, scope.Data );
        else
            return format::format( "{} {} {}\n{}", scope.Name, scope.Data, length, content );
    };

    String result;
    for( const Scope& child : m_root.Children ) {
        result += scopeToString( child );
    }
    return result;
}

template < typename T >
bool TextSerializer::doGenericWrite( T value ) {
    Scope& currentScope = *m_scopeStack.top();
    currentScope.Data += format::format( "{}", value );
    return true;
}

template < std::integral T >
bool TextSerializer::doGenericWrite( T value, uint8_t base ) {
    Scope& currentScope = *m_scopeStack.top();
    switch( base ) {
    case 2:
        currentScope.Data += format::format( "{:b}", value );
        break;
    case 8:
        currentScope.Data += format::format( "{:o}", value );
        break;
    case 10:
        currentScope.Data += format::format( "{}", value );
        break;
    case 16:
        currentScope.Data += format::format( "{:x}", value );
        break;
    default:
        ONYX_LOG_WARNING( "Unsupported base for json {}, falling back to base 10.", base );
        currentScope.Data += format::format( "{}", value );
        break;
    }

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
        valueAsBaseString = format::format( "{:b}", value );
        break;
    case 8:
        valueAsBaseString = format::format( "{:o}", value );
        break;
    case 10:
        valueAsBaseString = format::format( "{}", value );
        break;
    case 16:
        valueAsBaseString = format::format( "{:x}", value );
        break;
    default:
        ONYX_LOG_WARNING( "Unsupported base for json {}, falling back to base 10.", base );
        valueAsBaseString = format::format( "{}", value );
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
    }

    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    return true;
}

bool TextSerializer::createScope( uint64_t index ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() ) {
        currentScope.Children.resize( index + 1 );
    }

    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    return true;
}

bool TextSerializer::createScope( StringView name ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    Scope& currentScope = *m_scopeStack.top();
    auto it = std::ranges::find_if( currentScope.Children, [ name ]( Scope& scope ) { return name == scope.Name; } );
    uint64_t index = std::distance( std::begin( currentScope.Children ), it );
    if( it == currentScope.Children.end() ) {
        currentScope.Children.emplace_back( String( name ) );
    }

    m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    return true;
}

bool TextSerializer::endScope() {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    m_scopeStack.pop();
    return true;
}

} // namespace onyx::file_system
