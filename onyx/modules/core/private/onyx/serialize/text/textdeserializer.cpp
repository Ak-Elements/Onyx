#include <onyx/serialize/text/textdeserializer.h>

#include <onyx/serialize/text/textserializationinternal.h>
namespace onyx::serialization::text {

TextDeserializer::TextDeserializer( StringView data )
    : m_data( data )
    , m_root( makeUnique< internal::Scope >( 0, 0, 0, m_data.size() ) ) {
    m_scopeStack.emplace( m_root.get() );
    internal::initializeScope( m_data, *m_root );
}

TextDeserializer::~TextDeserializer() = default;

template < typename T >
bool TextDeserializer::doGenericRead( T& outValue ) const {
    const internal::Scope& currentScope = *m_scopeStack.top();
    [[maybe_unused]] StringView dataName = m_data.substr( currentScope.NameStart, currentScope.NameLength );
    StringView currentData = m_data.substr( currentScope.DataStart, currentScope.DataLength );

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
    } else if constexpr( std::is_same_v< Guid64, T > ) {
        StringView::size_type startIndex = currentData.find_first_not_of( " \t\n\r" );
        lastIndex = currentData.find_first_of( " \t\n\r", startIndex + 1 );
        StringView value = currentData.substr( startIndex, lastIndex - startIndex );

        uint64_t uid = 0;
        bool success = toNumeric( value, uid, 16 ) == std::errc();
        outValue = Guid64( uid );
        return success;
    } else if constexpr( IsVector< T > ) {
        using ScalarT = typename T::ScalarT;
        auto parseComponent = []( StringView::size_type& index, StringView data, ScalarT& outValue ) {
            StringView::size_type startIndex = data.find_first_not_of( " \t\n\r", index );
            StringView::size_type lastIndex = data.find_first_of( " \t\n\r", startIndex + 1 );
            StringView value = data.substr( startIndex, lastIndex - startIndex );
            bool success = toNumeric( value, outValue ) == std::errc();
            index = lastIndex;
            return success;
        };

        StringView::size_type index = 0;
        bool success = parseComponent( index, currentData, outValue.X );
        if( success == false )
            return false;

        success = parseComponent( index, currentData, outValue.Y );
        if( success == false )
            return false;

        if constexpr( IsVector3< T > ) {
            success = parseComponent( index, currentData, outValue.Z );
            if( success == false )
                return false;
        }

        if constexpr( IsVector4< T > ) {
            success = parseComponent( index, currentData, outValue.W );
            if( success == false )
                return false;
        }
    }

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
    const internal::Scope& currentScope = *m_scopeStack.top();
    StringView currentData = m_data.substr( currentScope.DataStart, currentScope.DataLength );

    StringView::size_type lastIndex = currentData.find_first_of( " \t\n\r" );
    StringView value = currentData.substr( 0, lastIndex );

    if( value[ 0 ] == '#' )
        value.remove_prefix( 1 );

    bool success = toNumeric( value, outValue, base ) == std::errc();
    return success;
}

template < Numeric T >
bool TextDeserializer::doGenericRead( StringView name, T& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool TextDeserializer::createScope( uint32_t index ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    internal::Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() )
        return false;

    internal::Scope* newScope = m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    if( newScope->IsLeaf == false ) {
        internal::initializeScope( m_data, *newScope );
    }
    return true;
}

bool TextDeserializer::createScope( uint64_t index ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    internal::Scope& currentScope = *m_scopeStack.top();
    if( index >= currentScope.Children.size() )
        return false;

    internal::Scope* newScope = m_scopeStack.emplace( &( currentScope.Children[ index ] ) );
    if( newScope->IsLeaf == false ) {
        internal::initializeScope( m_data, *newScope );
    }
    return true;
}

bool TextDeserializer::createScope( StringView name ) const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    internal::Scope& currentScope = *m_scopeStack.top();
    auto it = std::ranges::find_if( currentScope.Children, [ & ]( internal::Scope& scope ) {
        StringView scopeName = m_data.substr( scope.NameStart, scope.NameLength );
        return name == scopeName;
    } );

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
    const internal::Scope& currentScope = *m_scopeStack.top();
    outKey = m_data.substr( currentScope.NameStart, currentScope.NameLength );
    return true;
}

bool TextDeserializer::endScope() const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    m_scopeStack.pop();
    return true;
}

uint32_t TextDeserializer::getItemsCount() const {
    ONYX_ASSERT( m_scopeStack.empty() == false );
    const internal::Scope& currentScope = *m_scopeStack.top();
    return numericCast< uint32_t >( currentScope.Children.size() );
}

bool TextDeserializer::doRead( bool& outValue ) const {
    return doGenericRead( outValue );
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

bool TextDeserializer::doRead( StringView& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Guid64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2s8& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2s16& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2s32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2s64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2u8& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2u16& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2u32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2u64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2f32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector2f64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3s8& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3s16& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3s32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3s64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3u8& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3u16& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3u32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3u64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3f32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector3f64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4s8& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4s16& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4s32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4s64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4u8& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4u16& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4u32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4u64& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4f32& outValue ) const {
    return doGenericRead( outValue );
}

bool TextDeserializer::doRead( Vector4f64& outValue ) const {
    return doGenericRead( outValue );
}
} // namespace onyx::serialization::text
