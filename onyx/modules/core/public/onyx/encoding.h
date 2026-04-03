#pragma once

#include <utf8.h>

namespace onyx::encoding {
inline StringU16 utf8ToUtf16( StringView utf8Str ) {
    return utf8::utf8to16( utf8Str );
}

inline String utf16ToUtf8( StringViewU16 utf16Str ) {
    return utf8::utf16to8( utf16Str );
}

inline StringU32 utf8ToUtf32( StringView utf8Str ) {
    return utf8::utf8to32( utf8Str );
}
} // namespace onyx::encoding