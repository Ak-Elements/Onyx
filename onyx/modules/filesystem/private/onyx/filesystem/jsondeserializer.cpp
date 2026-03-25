#include <onyx/filesystem/jsondeserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system {
JsonDeserializer::JsonDeserializer( const nlohmann::ordered_json& json )
    : JsonRoot( json ) {
    JsonNodes.emplace( &JsonRoot );
}

template < typename T >
bool JsonDeserializer::DoGenericRead( T& outValue ) const {
    outValue = GetCurrent().get< T >();
    return true;
}

template < std::integral T >
bool JsonDeserializer::DoGenericRead( T& outValue, uint8_t base ) const {
    StringView valueAsBaseString = GetCurrent().get< StringView >();
    bool success = std::from_chars( valueAsBaseString.data(),
                                    valueAsBaseString.data() + valueAsBaseString.size(),
                                    outValue,
                                    base )
                       .ec == std::errc{};
    return success;
}

template < typename T >
bool JsonDeserializer::DoGenericRead( StringView name, T& outValue ) const {
    auto it = GetCurrent().find( name );
    if ( it == GetCurrent().end() ) {
        return false;
    }

    outValue = it->get< T >();
    return true;
}

template < std::integral T >
bool JsonDeserializer::DoGenericRead( StringView name, T& outValue, uint8_t base ) const {
    StringView valueAsBaseString = GetCurrent()[ name ].get< StringView >();
    bool success = std::from_chars( valueAsBaseString.data(),
                                    valueAsBaseString.data() + valueAsBaseString.size(),
                                    outValue,
                                    base )
                       .ec == std::errc{};
    return success;
}

bool JsonDeserializer::doRead( bool& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( StringView name, bool& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( int8_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( int16_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( int32_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( int64_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint8_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint16_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint32_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( uint64_t& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( float32& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( float64& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( int8_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int16_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int32_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( int64_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint8_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint16_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint32_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( uint64_t& outValue, uint8_t base ) const {
    return DoGenericRead( outValue, base );
}

bool JsonDeserializer::doRead( StringView name, int8_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, int16_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, int32_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, int64_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, uint8_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, uint16_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, uint32_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, uint64_t& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, float32& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, float64& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::doRead( StringView name, int8_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, int16_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, int32_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, int64_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, uint8_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, uint16_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, uint32_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView name, uint64_t& outValue, uint8_t base ) const {
    return DoGenericRead( name, outValue, base );
}

bool JsonDeserializer::doRead( StringView& outValue ) const {
    return DoGenericRead( outValue );
}

bool JsonDeserializer::doRead( StringView name, StringView& outValue ) const {
    return DoGenericRead( name, outValue );
}

bool JsonDeserializer::createScope( uint32_t index ) const {
    nlohmann::ordered_json& current = GetCurrent();
    if ( current.is_object() ) {
        uint32_t currentIndex = 0;
        for ( const auto& item : current.items() ) {
            if ( currentIndex == index ) {
                m_CurrentScopeName = item.key();
                JsonNodes.push( &( GetCurrent()[ item.key() ] ) );
                return true;
            }
            ++currentIndex;
        }

        return false;
    } else {
        JsonNodes.push( &( GetCurrent()[ index ] ) );
        return true;
    }
}

bool JsonDeserializer::createScope( uint64_t index ) const {
    nlohmann::ordered_json& current = GetCurrent();
    if ( current.is_object() ) {
        uint32_t currentIndex = 0;
        for ( const auto& item : current.items() ) {
            if ( currentIndex == index ) {
                m_CurrentScopeName = item.key();
                JsonNodes.push( &( GetCurrent()[ item.key() ] ) );
                return true;
            }
        }

        return false;
    } else {
        JsonNodes.push( &( GetCurrent()[ index ] ) );
        return true;
    }
}

bool JsonDeserializer::createScope( StringView name ) const {
    nlohmann::ordered_json& current = GetCurrent();
    if ( current.is_object() == false )
        return false;

    for ( auto it = current.begin(); it != current.end(); ++it ) {
        if ( ignoreCaseEqual( it.key(), name ) ) {
            m_CurrentScopeName = name;
            nlohmann::ordered_json& newScope = *it;
            JsonNodes.push( &newScope );
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
    bool success = std::from_chars( m_CurrentScopeName.data(),
                                    m_CurrentScopeName.data() + m_CurrentScopeName.size(),
                                    guid64,
                                    16 )
                       .ec == std::errc{};
    outKey = Guid64( guid64 );
    return success;
}

bool JsonDeserializer::getScopeIdentifier( StringView& outKey ) const {
    outKey = m_CurrentScopeName;
    return true;
}

bool JsonDeserializer::endScope() const {
    JsonNodes.pop();
    return true;
}

uint32_t JsonDeserializer::getItemsCount() const {
    return numericCast< uint32_t >( GetCurrent().size() );
}

nlohmann::ordered_json& JsonDeserializer::GetCurrent() {
    return *JsonNodes.top();
}

nlohmann::ordered_json& JsonDeserializer::GetCurrent() const {
    return *JsonNodes.top();
}
} // namespace onyx::file_system
