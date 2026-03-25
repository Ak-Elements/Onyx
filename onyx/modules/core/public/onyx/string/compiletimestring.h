#pragma once

namespace onyx {
template < uint64_t N >
struct CompileTimeString {
    consteval CompileTimeString() = default;
    consteval CompileTimeString( const char ( &str )[ N + 1 ] ) {
        for ( uint64_t i = 0; i < N + 1; ++i ) {
            Data[ i ] = str[ i ];
        }
    }

    ONYX_NO_DISCARD consteval uint64_t size() const { return N; }
    ONYX_NO_DISCARD consteval const char* data() const { return &Data[ 0 ]; }
    ONYX_NO_DISCARD consteval StringView stringView() const { return { &Data[ 0 ], N }; }

    template < uint64_t OtherN >
    consteval CompileTimeString< N + OtherN > operator+( const CompileTimeString< OtherN >& other ) const {
        CompileTimeString< N + OtherN > result;
        for ( int i = 0; i < N; ++i ) {
            result.Data[ i ] = Data[ i ];
        }

        for ( int i = 0; i < OtherN; ++i ) {
            result.Data[ N + i ] = other.Data[ i ];
        }

        result.Data[ N + OtherN ] = '\0';

        return result;
    }

    template < uint64_t OtherN >
    consteval auto operator+( const char ( &other )[ OtherN ] ) const {
        CompileTimeString< OtherN - 1 > otherCompileTimeStr( other );
        return *this + otherCompileTimeStr;
    }

    char Data[ N + 1 ]{}; // Array<char, N + 1> Data;
};

template < uint64_t N >
CompileTimeString( const char ( &str )[ N ] ) -> CompileTimeString< N - 1 >;
} // namespace onyx