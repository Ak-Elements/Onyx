#pragma once

namespace Onyx
{
    DynamicArray<String> Split(const StringView& string, const StringView& delimiters);
    DynamicArray<String> Split(const StringView& string, const char delimiter);

    void ToLower(String& str);
    String ToLower(const StringView& str);

    StringView Trim(const StringView& str);

    int IgnoreCaseCompare(const StringView& lhs, const StringView& rhs);
    bool IgnoreCaseEqual(const StringView& lhs, const StringView& rhs);
    StringView::size_type IgnoreCaseFind(const StringView& string, const StringView& searchString);

}