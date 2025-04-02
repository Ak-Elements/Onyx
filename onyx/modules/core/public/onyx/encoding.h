#pragma once

#include <utf8.h>

namespace Onyx::Encoding
{
    inline String_U16 utf8_to_utf16(StringView utf8Str)
    {
        return utf8::utf8to16(utf8Str);
    }

    inline String utf16_to_utf8(StringView_U16 utf16Str)
    {
        return utf8::utf16to8(utf16Str);
    }

    inline String_U32 utf8_to_utf32(StringView utf8Str)
    {
        return utf8::utf8to32(utf8Str);
    }
}