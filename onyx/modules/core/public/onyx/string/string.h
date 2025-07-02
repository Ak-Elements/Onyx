#pragma once

namespace Onyx
{
    DynamicArray<String> Split(StringView string, StringView delimiters);
    DynamicArray<String> Split(StringView string, const char delimiter);

    void ToLower(String& str);
    String ToLower(StringView str);

    StringView Trim(StringView str);
    StringView TrimLeft(StringView str);

    int IgnoreCaseCompare(StringView lhs, StringView rhs);
    bool IgnoreCaseEqual(StringView lhs, StringView rhs);
    bool IgnoreCaseStartsWith(StringView& string, const StringView prefix);
    StringView::size_type IgnoreCaseFind(StringView string, StringView searchString);

}