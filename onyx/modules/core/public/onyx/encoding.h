#pragma once

#include <utf8.h>

namespace onyx::encoding {
inline String_U16 utf8ToUtf16( StringView utf8Str ) {
    return utf8::utf8to16( utf8Str );
}

inline String utf16ToUtf8( StringView_U16 utf16Str ) {
    return utf8::utf16to8( utf16Str );
}

inline String_U32 utf8ToUtf32( StringView utf8Str ) {
    return utf8::utf8to32( utf8Str );
}
} // namespace onyx::encoding