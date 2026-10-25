#include <onyx/filesystem/jsondeserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system {
JsonDeserializer::JsonDeserializer( StringView json )
    : JsonRoot( nlohmann::json::parse( json, nullptr, false ) ) {
    m_jsonNodes.emplace( &JsonRoot );
}

template < typename T >
bool JsonDeserializer::doGenericRead( T& outValue ) const {
    if constexpr( std::is_same_v< T, Guid64 > ) {
        uint64_t guid64;
        StringView value = getCurrent().get< StringView >();
        bool success = std::from_chars( value.data(), value.data() + value.size(), guid64, 16 ).ec == std::errc{};
        outValue = Guid64( guid64 );
        return success;
    } else {
        outValue = getCurrent().get< T >();
    }
    return true;
}

template < Numeric T >
bool JsonDeserializer::doGenericRead( T& outValue, uint8_t base ) const {
    StringView valueAsBaseString = getCurrent().get< StringView >();
    std::errc error = toNumeric( valueAsBaseString, outValue, base );
    return error == std::errc();
}

template < typename T >
bool JsonDeserializer::doGenericRead( StringView name, T& outValue ) const {
    auto it = getCurrent().find( name );
    if( it == getCurrent().end() ) {
        return false;
    }

    if constexpr( std::is_same_v< T, Guid64 > ) {
        uint64_t guid64;
        StringView value = it->get< StringView >();
        bool success = std::from_chars( value.data(), value.data() + value.size(), guid64, 16 ).ec == std::errc{};
        outValue = Guid64( guid64 );
        return success;
    } else {
        outValue = it->get< T >();
    }
    return true;
}

template < Numeric T >
bool JsonDeserializer::doGenericRead( StringView name, T& outValue, uint8_t base ) const {
    StringView valueAsBaseString = getCurrent()[ name ].get< StringView >();
    std::errc error = toNumeric( valueAsBaseString, outValue, base );
    return error == std::errc();
}

bool JsonDeserializer::createScope( uint32_t index ) const {
    nlohmann::ordered_json& current = getCurrent();
    if( current.is_object() ) {
        uint32_t currentIndex = 0;
        for( const auto& item : current.items() ) {
            if( currentIndex == index ) {
                m_currentScopeName = item.key();
                m_jsonNodes.push( &( getCurrent()[ item.key() ] ) );
                return true;
            }
            ++currentIndex;
        }

        return false;
    } else {
        m_jsonNodes.push( &( getCurrent()[ index ] ) );
        return true;
    }
}

bool JsonDeserializer::createScope( uint64_t index ) const {
    nlohmann::ordered_json& current = getCurrent();
    if( current.is_object() ) {
        uint32_t currentIndex = 0;
        for( const auto& item : current.items() ) {
            if( currentIndex == index ) {
                m_currentScopeName = item.key();
                m_jsonNodes.push( &( getCurrent()[ item.key() ] ) );
                return true;
            }
        }

        return false;
    } else {
        m_jsonNodes.push( &( getCurrent()[ index ] ) );
        return true;
    }
}

bool JsonDeserializer::createScope( StringView name ) const {
    nlohmann::ordered_json& current = getCurrent();
    if( current.is_object() == false )
        return false;

    for( auto it = current.begin(); it != current.end(); ++it ) {
        if( ignoreCaseEqual( it.key(), name ) ) {
            m_currentScopeName = name;
            nlohmann::ordered_json& newScope = *it;
            m_jsonNodes.push( &newScope );
            return true;
        }
    }

    return false;
}

bool JsonDeserializer::getScopeIdentifier( uint32_t& /*outKey*/ ) const {
    ONYX_ASSERT( false, "Integral scope identifiers not supported in json" );
    return false;
}

bool JsonDeserializer::getScopeIdentifier( uint64_t& /*outKey*/ ) const {
    ONYX_ASSERT( false, "Integral scope identifiers not supported in json" );
    return false;
}

bool JsonDeserializer::getScopeIdentifier( Guid64& outKey ) const {
    uint64_t guid64 = 0;
    bool success = std::from_chars( m_currentScopeName.data(),
                                    m_currentScopeName.data() + m_currentScopeName.size(),
                                    guid64,
                                    16 )
                       .ec == std::errc{};
    outKey = Guid64( guid64 );
    return success;
}

bool JsonDeserializer::getScopeIdentifier( StringView& outKey ) const {
    outKey = m_currentScopeName;
    return true;
}

bool JsonDeserializer::endScope() const {
    m_jsonNodes.pop();
    return true;
}

uint32_t JsonDeserializer::getItemsCount() const {
    return numericCast< uint32_t >( getCurrent().size() );
}

nlohmann::ordered_json& JsonDeserializer::getCurrent() {
    return *m_jsonNodes.top();
}

nlohmann::ordered_json& JsonDeserializer::getCurrent() const {
    return *m_jsonNodes.top();
}

bool JsonDeserializer::doRead( bool& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( int8_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( int16_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( int32_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( int64_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint8_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint16_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint32_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint64_t& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( float32& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( float32& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( float64& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( float64& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int8_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int16_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int32_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int64_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint8_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint16_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint32_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint64_t& outValue, uint8_t base ) const {
    return doGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( StringView& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( Guid64& outValue ) const {
    return doGenericRead( outValue );
}

bool JsonDeserializer::doRead( Vector2s8& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2s16& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2s32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2s64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2u8& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2u16& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2u32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2u64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2f32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector2f64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    return success;
}

bool JsonDeserializer::doRead( Vector3s8& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3s16& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3s32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3s64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3u8& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3u16& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3u32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3u64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3f32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector3f64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    return success;
}

bool JsonDeserializer::doRead( Vector4s8& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4s16& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4s32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4s64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4u8& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4u16& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4u32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4u64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4f32& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}

bool JsonDeserializer::doRead( Vector4f64& outValue ) const {
    bool success = readAt( 0, outValue.X );
    success &= readAt( 1, outValue.Y );
    success &= readAt( 2, outValue.Z );
    success &= readAt( 3, outValue.W );
    return success;
}
} // namespace onyx::file_system
