#pragma once

namespace onyx
{
    DynamicArray<String> Split(StringView string, StringView delimiters);
    DynamicArray<String> Split(StringView string, char delimiter);

    void ToLower(String& str);
    String ToLower(StringView str);

    StringView Trim(StringView str);
    StringView TrimLeft(StringView str);

    int IgnoreCaseCompare(StringView lhs, StringView rhs);
    bool IgnoreCaseEqual(StringView lhs, StringView rhs);
    bool IgnoreCaseStartsWith(StringView string, StringView prefix);
    StringView::size_type IgnoreCaseFind(StringView string, StringView searchString);

    String Replace(StringView str, StringView search, StringView replace);

}