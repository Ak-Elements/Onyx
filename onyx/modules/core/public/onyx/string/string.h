#pragma once

namespace onyx {
DynamicArray< String > split( StringView string, StringView delimiters );
DynamicArray< String > split( StringView string, char delimiter );

void toLower( String& str );
String toLower( StringView str );

StringView trim( StringView str );
StringView trimLeft( StringView str );
StringView trimRight( StringView str );

int ignoreCaseCompare( StringView lhs, StringView rhs );
bool ignoreCaseEqual( StringView lhs, StringView rhs );
bool ignoreCaseStartsWith( StringView string, StringView prefix );
StringView::size_type ignoreCaseFind( StringView string, StringView searchString );

String replace( StringView str, StringView search, StringView replace );

template < typename T >
std::errc toNumeric( StringView str, T& outValue ) {
    auto [ _, error ] = std::from_chars( str.data(), str.data() + str.size(), outValue );
    return error;
}

template < typename T >
std::errc toNumeric( StringView str, T& outValue, int32_t base ) {
    if constexpr( std::is_floating_point_v< T > ) {
        if( base == 10 ) {
            auto [ _, error ] = std::from_chars( str.data(), str.data() + str.size(), outValue );
            return error;
        } else {
            using IntegralType = std::
                conditional_t< sizeof( T ) == 4, uint32_t, std::conditional_t< sizeof( T ) == 8, uint64_t, void > >;
            IntegralType bits = 0;
            auto [ _, error ] = std::from_chars( str.data(), str.data() + str.size(), bits, base );
            outValue = std::bit_cast< T >( bits );
            return error;
        }
    } else {
        auto [ _, error ] = std::from_chars( str.data(), str.data() + str.size(), outValue, base );
        return error;
    }
}

} // namespace onyx
