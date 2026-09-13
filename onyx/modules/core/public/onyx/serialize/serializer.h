#pragma once

#include <onyx/geometry/rectserialization.h>
#include <onyx/geometry/vectorserialization.h>
#include <onyx/guid.h>
#include <onyx/serialize/serialization.h>
#include <onyx/stringid.h>

namespace onyx {
class Serializer {
    friend struct SerializationScope;

  public:
    Serializer() = default;
    virtual ~Serializer() = default;

    template < std::integral T >
    bool write( const T& value, uint8_t base ) {
        return doWrite( value, base );
    }

    template < CompileTimeString Name, std::integral T >
    bool write( const T& value, uint8_t base ) {
        return doWrite( Name.stringView(), value, base );
    }

    template < std::integral T >
    bool write( StringView name, const T& value, uint8_t base ) {
        return doWrite( name, value, base );
    }

    template < typename T >
    bool writeAt( uint32_t i, const T& value ) {
        if( createScope( i ) == false ) {
            return false;
        }

        bool success = write( value );
        success &= endScope();
        return success;
    }

    template < typename T >
    bool write( const T& value ) {
        if constexpr( std::is_fundamental_v< T > || std::is_same_v< T, StringView > ) {
            return doWrite( value );
        } else if constexpr( std::is_same_v< T, String > ) {
            return doWrite( StringView( value ) );
        } else if constexpr( std::is_enum_v< T > ) {
            bool success = doWrite( enums::toString< T >( value ) );
            return success;
        } else {
            bool success = Serialization< T >::serialize( *this, value );
            return success;
        }
    }

    template < CompileTimeString Name, typename T >
    bool write( const T& value ) {
        return write( Name.stringView(), value );
    }

    template < typename T >
    bool write( StringView name, const T& value ) {
        if constexpr( std::is_fundamental_v< T > || std::is_same_v< T, StringView > ) {
            return doWrite( name, value );
        } else if constexpr( std::is_same_v< T, String > ) {
            return doWrite( name, StringView( value ) );
        } else if constexpr( std::is_enum_v< T > ) {
            bool success = doWrite( name, enums::toString< T >( value ) );
            return success;
        } else {
            if( createScope( name ) == false ) {
                return false;
            }

            bool success = write( value );
            success &= endScope();
            return success;
        }
    }

    template < typename T, size_t N >
    bool write( const Array< T, N >& value ) {
        bool success = true;

        for( uint32_t i = 0; i < static_cast< uint32_t >( N ); ++i ) {
            if( createScope( i ) == false ) {
                return false;
            }
            success = write( value[ i ] );
            success &= endScope();
            if( success == false ) {
                break;
            }
        }

        return success;
    }

    template < typename T >
    bool write( const DynamicArray< T >& value ) {
        bool success = true;
        uint32_t count = numericCast< uint32_t >( value.size() );

        if( writeItemsCount( count ) == false )
            return false;

        for( uint32_t i = 0; i < count; ++i ) {
            if( createScope( i ) == false ) {
                return false;
            }
            success = write( value[ i ] );
            success &= endScope();
            if( success == false ) {
                break;
            }
        }

        return success;
    }

    template < typename T >
    bool write( StringView name, const DynamicArray< T >& value ) {
        if( value.empty() ) {
            return true;
        }

        if( createScope( name ) == false ) {
            return false;
        }

        bool success = write( value );
        success &= endScope();
        return success;
    }

    template < typename T, uint8_t Size >
    bool write( StringView name, const InplaceArray< T, Size >& value ) {
        if( value.empty() ) {
            return true;
        }

        if( createScope( name ) == false ) {
            return false;
        }

        bool success = true;
        uint8_t count = value.size();

        if( writeItemsCount( count ) == false )
            return false;

        for( uint8_t i = 0; i < count; ++i ) {
            success = createScope( static_cast< uint32_t >( i ) );
            if( success == false ) {
                break;
            }

            success &= write( value[ i ] );
            success &= endScope();

            if( success == false ) {
                break;
            }
        }

        success &= endScope();
        return success;
    }

    template < typename KeyT, typename ValueT >
    bool write( const HashMap< KeyT, ValueT >& map ) {
        // write out count
        bool success = true;

        if( writeItemsCount( numericCast< uint64_t >( map.size() ) ) == false )
            return false;

        for( auto&& [ key, value ] : map ) {
            if constexpr( std::is_integral_v< KeyT > || IsStringId< KeyT > || IsGuid64< KeyT > ) {
                if( isSupportingIntegralScopes() ) {
                    auto scopeKey = getScopeKeyUnderlyingType( key );
                    success = createScope( scopeKey );
                    if( success == false ) {
                        break;
                    }

                    success &= Serializer::template write< ValueT >( value );
                    success &= endScope();

                    if( success == false ) {
                        break;
                    }

                    continue;
                }
            }

            StringView keyStr = scopeKeyToString( key );
            if( Serializer::template write< ValueT >( keyStr, value ) == false ) {
                return false;
            }
        }

        return success;
    }

    template < CompileTimeString Name, typename T, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, Serializer&, const T& >
    bool writeForEach( const DynamicArray< T >& value, Callable forEachFunctor ) {
        if( createScope( Name.stringView() ) == false ) {
            return false;
        }

        bool success = writeForEach( value, forEachFunctor );
        success &= endScope();
        return success;
    }

    template < typename T, typename Callable > requires std::is_invocable_r_v< bool, Callable, Serializer&, const T& >
    bool writeForEach( const DynamicArray< T >& value, Callable forEachFunctor ) {
        bool success = true;
        uint32_t count = numericCast< uint32_t >( value.size() );

        if( writeItemsCount( count ) == false )
            return false;

        for( uint32_t i = 0; i < count; ++i ) {
            if( createScope( i ) == false ) {
                return false;
            }

            success = forEachFunctor( *this, value[ i ] );
            success &= endScope();
            if( success == false ) {
                break;
            }
        }

        return success;
    }

    template < typename Callable > requires std::is_invocable_r_v< bool, Callable, Serializer&, uint32_t >
    bool writeForEach( Callable forEachFunctor, uint32_t count ) {
        bool success = true;

        if( writeItemsCount( count ) == false )
            return false;

        for( uint32_t i = 0; i < count; ++i ) {
            if( createScope( i ) == false ) {
                return false;
            }

            success = forEachFunctor( *this, i );
            success &= endScope();
            if( success == false ) {
                break;
            }
        }

        return success;
    }

    template < CompileTimeString Name, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, Serializer&, uint32_t >
    bool writeForEach( Callable forEachFunctor, uint32_t count ) {
        if( count == 0 ) {
            return true;
        }

        if( createScope( Name.stringView() ) == false ) {
            return false;
        }

        bool success = writeForEach( forEachFunctor, count );
        success &= endScope();
        return success;
    }

    template < CompileTimeString Name, typename KeyT, typename ValueT, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, Serializer&, const KeyT&, const ValueT& >
    bool writeForEach( const HashMap< KeyT, ValueT >& map, Callable forEachFunctor ) {
        if( map.empty() ) {
            return true;
        }

        if( createScope( Name.stringView() ) == false ) {
            return false;
        }

        bool success = writeForEach( map, forEachFunctor );
        success &= endScope();
        return success;
    }

    template < typename KeyT, typename ValueT, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, Serializer&, const KeyT&, const ValueT& >
    bool writeForEach( const HashMap< KeyT, ValueT >& map, Callable forEachFunctor ) {
        bool success = true;

        if( writeItemsCount( numericCast< uint64_t >( map.size() ) ) == false )
            return false;

        for( auto&& [ key, value ] : map ) {
            if constexpr( std::is_integral_v< KeyT > || IsStringId< KeyT > || IsGuid64< KeyT > ) {
                if( isSupportingIntegralScopes() ) {
                    auto scopeKey = getScopeKeyUnderlyingType( key );
                    success = createScope( scopeKey );
                    if( success == false ) {
                        break;
                    }

                    success &= forEachFunctor( *this, key, value );
                    success &= endScope();

                    if( success == false ) {
                        break;
                    }

                    continue;
                }
            }

            StringView keyStr = scopeKeyToString( key );
            success = createScope( keyStr );
            if( success == false ) {
                break;
            }

            success &= forEachFunctor( *this, key, value );
            success &= endScope();
            if( success == false ) {
                break;
            }
        }

        return success;
    }

    template < CompileTimeString Name >
    SerializationScope enterScope() {
        return { *this, Name.stringView() };
    }
    SerializationScope enterScope( uint32_t i ) { return { *this, i }; }

  private:
    virtual bool doWrite( bool value ) = 0;
    virtual bool doWrite( StringView name, bool value ) = 0;

    virtual bool doWrite( int8_t value ) = 0;
    virtual bool doWrite( int16_t value ) = 0;
    virtual bool doWrite( int32_t value ) = 0;
    virtual bool doWrite( int64_t value ) = 0;
    virtual bool doWrite( uint8_t value ) = 0;
    virtual bool doWrite( uint16_t value ) = 0;
    virtual bool doWrite( uint32_t value ) = 0;
    virtual bool doWrite( uint64_t value ) = 0;
    virtual bool doWrite( int8_t value, uint8_t base ) = 0;
    virtual bool doWrite( int16_t value, uint8_t base ) = 0;
    virtual bool doWrite( int32_t value, uint8_t base ) = 0;
    virtual bool doWrite( int64_t value, uint8_t base ) = 0;
    virtual bool doWrite( uint8_t value, uint8_t base ) = 0;
    virtual bool doWrite( uint16_t value, uint8_t base ) = 0;
    virtual bool doWrite( uint32_t value, uint8_t base ) = 0;
    virtual bool doWrite( uint64_t value, uint8_t base ) = 0;

    virtual bool doWrite( StringView name, int8_t value ) = 0;
    virtual bool doWrite( StringView name, int16_t value ) = 0;
    virtual bool doWrite( StringView name, int32_t value ) = 0;
    virtual bool doWrite( StringView name, int64_t value ) = 0;
    virtual bool doWrite( StringView name, uint8_t value ) = 0;
    virtual bool doWrite( StringView name, uint16_t value ) = 0;
    virtual bool doWrite( StringView name, uint32_t value ) = 0;
    virtual bool doWrite( StringView name, uint64_t value ) = 0;
    virtual bool doWrite( StringView name, int8_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, int16_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, int32_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, int64_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, uint8_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, uint16_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, uint32_t value, uint8_t base ) = 0;
    virtual bool doWrite( StringView name, uint64_t value, uint8_t base ) = 0;

    virtual bool doWrite( float32 value ) = 0;
    virtual bool doWrite( float64 value ) = 0;
    virtual bool doWrite( StringView name, float32 value ) = 0;
    virtual bool doWrite( StringView name, float64 value ) = 0;

    virtual bool doWrite( StringView value ) = 0;
    virtual bool doWrite( StringView name, StringView value ) = 0;

    virtual bool createScope( uint32_t index ) = 0;
    virtual bool createScope( uint64_t index ) = 0;
    virtual bool createScope( StringView name ) = 0;

    virtual bool endScope() = 0;

    virtual bool writeItemsCount( uint8_t count ) = 0;
    virtual bool writeItemsCount( uint16_t count ) = 0;
    virtual bool writeItemsCount( uint32_t count ) = 0;
    virtual bool writeItemsCount( uint64_t count ) = 0;

    [[nodiscard]] virtual bool isSupportingIntegralScopes() const = 0;

    template < typename T >
    auto getScopeKeyUnderlyingType( T key ) {
        if constexpr( IsStringId< T > ) {
            return key.getId();
        } else if constexpr( IsGuid64< T > ) {
            return key.get();
        } else {
            return key;
        }
    }

    template < typename T >
    StringView scopeKeyToString( T key ) {
        if constexpr( IsStringId< T > ) {
            return format::format( "{:x}", key.getId() );
        } else if constexpr( IsGuid64< T > ) {
            return format::format( "{:x}", key.get() );
        } else {
            return format::format( "{}", key );
        }
    }
};

} // namespace onyx
