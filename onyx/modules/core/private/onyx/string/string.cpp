#include <onyx/string/string.h>

namespace onyx {
DynamicArray< String > split( StringView string, StringView delimiters ) {
    DynamicArray< String > result;

    if( string.empty() )
        return result;

    const uint64_t length = string.length();
    bool isEscaped = false;

    result.emplace_back();
    uint32_t currentTokenIndex = 0;
    for( uint64_t i = 0; i < length; ++i ) {
        if( string[ i ] == '\\' ) {
            isEscaped = !isEscaped;
            continue;
        }

        if( ( isEscaped == false ) && delimiters.find( string[ i ] ) != StringView::npos ) {
            if( result[ currentTokenIndex ].empty() == false ) {
                result.emplace_back();
                ++currentTokenIndex;
            }
        } else {
            isEscaped = false;
            result[ currentTokenIndex ] += string[ i ];
        }
    }

    return result;
}

DynamicArray< String > split( StringView string, char delimiter ) {
    return split( string, String( 1, delimiter ) );
}

bool ignoreCaseEqual( StringView lhs, StringView rhs ) {
    return std::ranges::equal( lhs, rhs, []( char lhs, char rhs ) {
        return std::tolower( lhs ) == std::tolower( rhs );
    } );
}

bool ignoreCaseStartsWith( StringView string, StringView prefix ) {
    return toLower( string ).starts_with( toLower( prefix ) );
}

StringView::size_type ignoreCaseFind( StringView string, StringView searchString ) {
    auto it = std::ranges::search( string, searchString, []( char lhs, char rhs ) {
                  return std::tolower( lhs ) == std::tolower( rhs );
              } ).begin();
    if( it != string.end() )
        return it - string.begin();
    else
        return StringView::npos;
}

String replace( StringView str, StringView search, StringView replace ) {
    return str | std::views::split( search ) | std::views::join_with( replace ) | std::ranges::to< String >();
}

// move to string header
void toLower( String& str ) {
    std::ranges::transform( str, str.begin(), []( unsigned char c ) {
        return static_cast< char >( std::tolower( c ) );
    } );
}

String toLower( StringView str ) {
    String result( str );
    toLower( result );
    return result;
}

StringView trim( StringView str ) {
    if( str.empty() )
        return str;

    StringView::size_type start = str.find_first_not_of( "\n\r\t " );
    StringView::size_type end = str.find_last_not_of( "\n\r\t " );

    if( ( start == StringView::npos ) && ( end == StringView::npos ) ) {
        return "";
    }

    return str.substr( start, ( end - start ) + 1 );
}

StringView trimLeft( StringView str ) {
    if( str.empty() )
        return str;

    StringView::size_type start = str.find_first_not_of( " \n\r\t" );

    if( start == StringView::npos ) {
        return str;
    }

    return str.substr( start );
}
} // namespace onyx
