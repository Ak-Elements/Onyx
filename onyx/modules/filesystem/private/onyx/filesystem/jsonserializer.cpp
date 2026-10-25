#include <onyx/filesystem/jsonserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system {
JsonSerializer::JsonSerializer()
    : JsonRoot( nlohmann::ordered_json{} ) {
    JsonNodes.push( &JsonRoot );
}

template < typename T >
bool JsonSerializer::doGenericWrite( T value ) {
    getCurrent() = value;
    return true;
}

template < std::integral T >
bool JsonSerializer::doGenericWrite( T value, uint8_t base ) {
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

    getCurrent() = valueAsBaseString;
    return true;
}

template < typename T >
bool JsonSerializer::doGenericWrite( StringView name, T value ) {
    getCurrent()[ name ] = value;
    return true;
}

template < std::integral T >
bool JsonSerializer::doGenericWrite( StringView name, T value, uint8_t base ) {
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

    getCurrent()[ name ] = valueAsBaseString;
    return true;
}

bool JsonSerializer::doWrite( bool value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( int8_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( int16_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( int32_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( int64_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( uint8_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( uint16_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( uint32_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( uint64_t value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( float32 value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( float64 value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::doWrite( int8_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( int16_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( int32_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( int64_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint8_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint16_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint32_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( uint64_t value, uint8_t base ) {
    return doGenericWrite( value, base );
}

bool JsonSerializer::doWrite( StringView value ) {
    return doGenericWrite( value );
}

bool JsonSerializer::createScope( uint32_t index ) {
    nlohmann::ordered_json& current = getCurrent();
    if( current.is_object() ) {
        StringView scopeKey = format::format( "{:x}", index );
        return createScope( scopeKey );
    } else {
        JsonNodes.push( &( getCurrent()[ index ] ) );
        return true;
    }
}

bool JsonSerializer::createScope( uint64_t index ) {
    nlohmann::ordered_json& current = getCurrent();
    if( current.is_object() ) {
        StringView scopeKey = format::format( "{:x}", index );
        return createScope( scopeKey );
    } else {
        JsonNodes.push( &( getCurrent()[ index ] ) );
        return true;
    }
}

bool JsonSerializer::createScope( StringView name ) {
    nlohmann::ordered_json& current = getCurrent();
    nlohmann::ordered_json& newCurrent = current[ name ];

    m_CurrentScopeName = name;
    JsonNodes.push( &newCurrent );
    return true;
}

bool JsonSerializer::endScope() {
    JsonNodes.pop();
    return true;
}

nlohmann::ordered_json& JsonSerializer::getCurrent() {
    return *JsonNodes.top();
}

nlohmann::ordered_json& JsonSerializer::getCurrent() const {
    return *JsonNodes.top();
}

bool JsonSerializer::doWrite( Vector2s8 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2s16 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2s32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2s64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2u8 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2u16 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2u32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2u64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2f32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector2f64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    return success;
}

bool JsonSerializer::doWrite( Vector3s8 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3s16 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3s32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3s64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3u8 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3u16 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3u32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3u64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3f32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector3f64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    return success;
}

bool JsonSerializer::doWrite( Vector4s8 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4s16 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4s32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4s64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4u8 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4u16 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4u32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4u64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4f32 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

bool JsonSerializer::doWrite( Vector4f64 value ) {
    bool success = writeAt( 0, value.X );
    success &= writeAt( 1, value.Y );
    success &= writeAt( 2, value.Z );
    success &= writeAt( 3, value.W );
    return success;
}

} // namespace onyx::file_system
