#pragma once

namespace onyx {
DynamicArray< String > split( StringView string, StringView delimiters );
DynamicArray< String > split( StringView string, char delimiter );

void toLower( String& str );
String toLower( StringView str );

StringView trim( StringView str );
StringView trimLeft( StringView str );

int ignoreCaseCompare( StringView lhs, StringView rhs );
bool ignoreCaseEqual( StringView lhs, StringView rhs );
bool ignoreCaseStartsWith( StringView string, StringView prefix );
StringView::size_type ignoreCaseFind( StringView string, StringView searchString );

String replace( StringView str, StringView search, StringView replace );

} // namespace onyx