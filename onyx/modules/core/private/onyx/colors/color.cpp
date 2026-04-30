#include <onyx/colors/color.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
bool Serialization< Color >::serialize( Serializer& serializer, const Color& value ) {
    return serializer.write( value.toRGBA(), 16 );
}

bool Serialization< Color >::deserialize( const Deserializer& deserializer, Color& outValue ) {
    uint32_t color = 0;
    if( deserializer.read( color, 16 ) ) {
        outValue = Color( color );
        return true;
    }

    return false;
}
} // namespace onyx
