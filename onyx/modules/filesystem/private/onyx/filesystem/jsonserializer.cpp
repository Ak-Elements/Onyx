#include <onyx/filesystem/jsonserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system {
JsonSerializer::JsonSerializer()
    : JsonRoot( nlohmann::ordered_json{} ) {
    JsonNodes.push( &JsonRoot );
}

template < typename T >
bool JsonSerializer::DoGenericWrite( T value ) {
    GetCurrent() = value;
    return true;
}

template < std::integral T >
bool JsonSerializer::DoGenericWrite( T value, uint8_t base ) {
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

    GetCurrent() = valueAsBaseString;
    return true;
}

template < typename T >
bool JsonSerializer::DoGenericWrite( StringView name, T value ) {
    GetCurrent()[ name ] = value;
    return true;
}

template < std::integral T >
bool JsonSerializer::DoGenericWrite( StringView name, T value, uint8_t base ) {
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

    GetCurrent()[ name ] = valueAsBaseString;
    return true;
}

bool JsonSerializer::doWrite( bool value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( StringView name, bool value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( int8_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( int16_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( int32_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( int64_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( uint8_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( uint16_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( uint32_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( uint64_t value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( float32 value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( float64 value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( int8_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( int16_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( int32_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( int64_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint8_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint16_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint32_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint64_t value, uint8_t base ) {
    return DoGenericWrite( value, base );
}

bool JsonSerializer::doWrite( StringView name, int8_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, int16_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, int32_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, int64_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, uint8_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, uint16_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, uint32_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, uint64_t value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, float32 value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, float64 value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::doWrite( StringView name, int8_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, int16_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, int32_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, int64_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, uint8_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, uint16_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, uint32_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView name, uint64_t value, uint8_t base ) {
    return DoGenericWrite( name, value, base );
}

bool JsonSerializer::doWrite( StringView value ) {
    return DoGenericWrite( value );
}

bool JsonSerializer::doWrite( StringView name, StringView value ) {
    return DoGenericWrite( name, value );
}

bool JsonSerializer::createScope( uint32_t index ) {
    nlohmann::ordered_json& current = GetCurrent();
    if( current.is_object() ) {
        StringView scopeKey = format::format( "{:x}", index );
        return createScope( scopeKey );
    } else {
        JsonNodes.push( &( GetCurrent()[ index ] ) );
        return true;
    }
}

bool JsonSerializer::createScope( uint64_t index ) {
    nlohmann::ordered_json& current = GetCurrent();
    if( current.is_object() ) {
        StringView scopeKey = format::format( "{:x}", index );
        return createScope( scopeKey );
    } else {
        JsonNodes.push( &( GetCurrent()[ index ] ) );
        return true;
    }
}

bool JsonSerializer::createScope( StringView name ) {
    nlohmann::ordered_json& current = GetCurrent();
    nlohmann::ordered_json& newCurrent = current[ name ];

    m_CurrentScopeName = name;
    JsonNodes.push( &newCurrent );
    return true;
}

bool JsonSerializer::endScope() {
    JsonNodes.pop();
    return true;
}

nlohmann::ordered_json& JsonSerializer::GetCurrent() {
    return *JsonNodes.top();
}

nlohmann::ordered_json& JsonSerializer::GetCurrent() const {
    return *JsonNodes.top();
}
} // namespace onyx::file_system
