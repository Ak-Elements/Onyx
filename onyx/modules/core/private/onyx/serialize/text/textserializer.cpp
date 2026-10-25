#include <onyx/serialize/text/textserializer.h>

#include <onyx/serialize/text/textserializationinternal.h>

namespace onyx::serialization::text {

TextSerializer::TextSerializer()
    : m_root( makeUnique< internal::Scope >( 0, 0, 0, 0 ) ) {
    m_scopeStack.emplace( m_root.get() );
}

TextSerializer::TextSerializer( String&& data )
    : m_data( std::move( data ) )
    , m_root( makeUnique< internal::Scope >( 0, 0, 0, m_data.size() ) ) {
    m_scopeStack.emplace( m_root.get() );
    internal::initializeScope( m_data, *m_root );
}

TextSerializer::~TextSerializer() = default;

template < typename T >
bool TextSerializer::doGenericWrite( T value ) {
    internal::Scope& currentScope = *m_scopeStack.top();

    [[maybe_unused]] StringView::size_type oldLength = currentScope.DataLength;

    StringView newData = format::format( "{}\n", value );
    [[maybe_unused]] const size_t newDataLength = newData.size();

    m_data.replace( currentScope.DataStart, currentScope.DataLength, newData );
    currentScope.DataLength = newDataLength;
    return true;
}

template < std::integral T >
bool TextSerializer::doGenericWrite( T value, uint8_t base ) {
    internal::Scope& currentScope = *m_scopeStack.top();
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

bool TextSerializer::createScope( uint32_t index ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    internal::Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() ) {
        currentScope.Children.resize( index + 1 );
        internal::Scope& newScope = currentScope.Children[ index ];
        StringView newScopeIndexString = format::format( "{}", index );
        m_data += newScopeIndexString;
        newScope.NameStart = currentScope.DataStart + currentScope.DataLength;
        newScope.NameLength = newScopeIndexString.size();
        newScope.DataStart = newScope.NameStart + newScope.NameLength;
        m_scopeStack.emplace( &newScope );
    } else {
        internal::Scope& existingScope = currentScope.Children[ index ];
        if( existingScope.IsLeaf == false ) {
            internal::initializeScope( m_data, existingScope );
        }

        m_scopeStack.emplace( &existingScope );
    }

    return true;
}

bool TextSerializer::createScope( uint64_t index ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    internal::Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() ) {
        currentScope.Children.resize( index + 1 );
        internal::Scope& newScope = currentScope.Children[ index ];
        StringView newScopeIndexString = format::format( "{}", index );
        m_data += newScopeIndexString;
        newScope.NameStart = currentScope.DataStart + currentScope.DataLength;
        newScope.NameLength = newScopeIndexString.size();
        newScope.DataStart = newScope.NameStart + newScope.NameLength;
        m_scopeStack.emplace( &newScope );
    } else {
        internal::Scope& existingScope = currentScope.Children[ index ];
        if( existingScope.IsLeaf == false ) {
            internal::initializeScope( m_data, existingScope );
        }

        m_scopeStack.emplace( &existingScope );
    }
    return true;
}

bool TextSerializer::createScope( StringView name ) {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    internal::Scope& currentScope = *m_scopeStack.top();
    auto it = std::ranges::find_if( currentScope.Children, [ & ]( internal::Scope& scope ) {
        return name == StringView( m_data ).substr( scope.NameStart, scope.NameLength );
    } );
    uint64_t index = std::distance( std::begin( currentScope.Children ), it );
    if( it == currentScope.Children.end() ) {
        auto currentScopeEnd = currentScope.DataStart + currentScope.DataLength;
        internal::Scope newScope{ currentScopeEnd, name.size() };
        m_data += name;
        newScope.DataStart = currentScopeEnd + newScope.NameLength;
        newScope.DataLength = 0;
        currentScope.Children.emplace_back( newScope );
        m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    } else {
        internal::Scope& existingScope = currentScope.Children[ index ];
        if( existingScope.IsLeaf == false ) {
            internal::initializeScope( m_data, existingScope );
        }

        m_scopeStack.emplace( &existingScope );
    }

    return true;
}

bool TextSerializer::endScope() {
    ONYX_ASSERT( m_scopeStack.empty() == false );

    internal::Scope& currentScope = *m_scopeStack.top();
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

        for( internal::Scope& child : currentScope.Children ) {
            updateScopeRange( child, deltaSize );
        }
    }

    m_scopeStack.pop();

    // adjust parent scope length
    internal::Scope& parentScope = *m_scopeStack.top();
    parentScope.DataLength = ( currentScope.DataStart + currentScope.DataLength ) - parentScope.DataStart;
    if( m_scopeStack.size() == 1 ) {
        m_data.resize( parentScope.DataLength );
    }
    return true;
}

bool TextSerializer::doWrite( bool value ) {
    return doGenericWrite( value );
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

bool TextSerializer::doWrite( StringView value ) {
    return doGenericWrite( value );
}

bool TextSerializer::doWrite( Vector2s8 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2s16 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2s32 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2s64 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2u8 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2u16 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2u32 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2u64 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2f32 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector2f64 value ) {
    return doWrite( format::format( "{} {}", value.X, value.Y ) );
}

bool TextSerializer::doWrite( Vector3s8 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3s16 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3s32 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3s64 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3u8 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3u16 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3u32 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3u64 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3f32 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector3f64 value ) {
    return doWrite( format::format( "{} {} {}", value.X, value.Y, value.Z ) );
}

bool TextSerializer::doWrite( Vector4s8 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4s16 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4s32 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4s64 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4u8 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4u16 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4u32 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4u64 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4f32 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

bool TextSerializer::doWrite( Vector4f64 value ) {
    return doWrite( format::format( "{} {} {} {}", value.X, value.Y, value.Z, value.W ) );
}

} // namespace onyx::serialization::text
