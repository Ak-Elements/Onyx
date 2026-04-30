#include <onyx/filesystem/textserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system {

template < typename T >
bool TextSerializer::doGenericWrite( T value ) {
    // GetCurrent() = value;
    return true;
}

template < std::integral T >
bool TextSerializer::doGenericWrite( T value, uint8_t base ) {
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

    // GetCurrent() = valueAsBaseString;
    return true;
}

template < typename T >
bool TextSerializer::doGenericWrite( StringView name, T value ) {
    // GetCurrent()[ name ] = value;
    return true;
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

    // GetCurrent()[ name ] = valueAsBaseString;
    return true;
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

bool TextSerializer::createScope( uint32_t /*index*/ ) {
    return true;
}

bool TextSerializer::createScope( uint64_t /*index*/ ) {
    return true;
}

bool TextSerializer::createScope( StringView /*name*/ ) {
    return true;
}

bool TextSerializer::endScope() {
    return true;
}

} // namespace onyx::file_system
