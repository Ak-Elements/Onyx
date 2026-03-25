#pragma once

#include <onyx/geometry/rectserialization.h>
#include <onyx/geometry/vectorserialization.h>
#include <onyx/guid.h>
#include <onyx/serialize/serialization.h>
#include <onyx/stringid.h>

namespace onyx {
class Deserializer {
  public:
    Deserializer() = default;
    virtual ~Deserializer() = default;

    template < CompileTimeString Name, typename T >
    bool read( T& outValue ) const {
        return read( Name.stringView(), outValue );
    }

    template < CompileTimeString Name, typename T >
    bool readOptional( T& outValue ) const {
        if ( createScope( Name.stringView() ) == false ) {
            return true;
        }

        bool success = read( outValue );
        success &= endScope();
        return success;
    }

    template < typename T >
    bool readAt( uint32_t i, T& outValue ) const {
        if ( createScope( i ) == false ) {
            return false;
        }

        bool success = read( outValue );
        success &= endScope();
        return success;
    }

    template < typename T >
    bool read( T& outValue ) const {
        if constexpr ( std::is_fundamental_v< T > || std::is_same_v< T, StringView > ) {
            return doRead( outValue );
        } else if constexpr ( std::is_same_v< T, String > ) {
            StringView stringView;
            bool success = doRead( stringView );
            outValue = stringView;
            return success;
        } else if constexpr ( std::is_enum_v< T > ) {
            StringView enumValue;
            if ( doRead( enumValue ) ) {
                outValue = enums::fromString< T >( enumValue );
                return true;
            }
            return false;
        } else {
            bool success = Serialization< T >::deserialize( *this, outValue );
            return success;
        }
    }

    template < typename T >
    bool read( StringView name, T& outValue ) const {
        if constexpr ( std::is_fundamental_v< T > || std::is_same_v< T, StringView > ) {
            return doRead( name, outValue );
        } else if constexpr ( std::is_same_v< T, String > ) {
            StringView stringView;
            bool success = doRead( name, stringView );
            outValue = stringView;
            return success;
        } else if constexpr ( std::is_enum_v< T > ) {
            StringView enumValue;
            if ( doRead( name, enumValue ) ) {
                outValue = enums::fromString< T >( enumValue );
                return true;
            }

            return false;
        } else {
            if ( createScope( name ) == false ) {
                return false;
            }

            bool success = read( outValue );
            success &= endScope();

            return success;
        }
    }

    template < std::integral T >
    bool read( T& outValue, uint8_t base ) const {
        return doRead( outValue, base );
    }
    template < CompileTimeString Name, std::integral T >
    bool read( T& outValue, uint8_t base ) const {
        return doRead( Name.stringView(), outValue, base );
    }

    template < typename T >
    bool read( DynamicArray< T >& outValue ) const {
        bool success = false;
        uint32_t count = getItemsCount();
        outValue.reserve( count );
        for ( uint32_t i = 0; i < count; ++i ) {
            if ( createScope( i ) == false ) {
                return false;
            }

            T& arrayValue = outValue.emplace_back();
            success = read( arrayValue );
            success &= endScope();
        }

        return success;
    }

    template < typename T, typename Callable > requires std::is_invocable_r_v< bool, Callable, const Deserializer&, T& >
    bool readForEach( DynamicArray< T >& outValue, Callable forEachFunctor ) const {
        bool success = false;
        uint32_t count = getItemsCount();
        outValue.reserve( count );
        for ( uint32_t i = 0; i < count; ++i ) {
            if ( createScope( i ) == false ) {
                return false;
            }

            T& arrayValue = outValue.emplace_back();
            success = forEachFunctor( *this, arrayValue );
            success &= endScope();
        }

        return success;
    }

    template < CompileTimeString Name, typename T, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, const Deserializer&, T& >
    bool readForEach( DynamicArray< T >& outValue, Callable forEachFunctor ) const {
        if ( createScope( Name.stringView() ) == false ) {
            return false;
        }

        bool success = false;
        uint32_t count = getItemsCount();
        outValue.reserve( count );
        for ( uint32_t i = 0; i < count; ++i ) {
            if ( createScope( i ) == false ) {
                return false;
            }

            T& arrayValue = outValue.emplace_back();
            success = forEachFunctor( *this, arrayValue );
            success &= endScope();
        }

        success &= endScope();
        return success;
    }

    template < typename Callable > requires std::is_invocable_r_v< bool, Callable, const Deserializer& >
    bool readForEach( Callable forEachFunctor ) const {
        bool success = false;
        uint32_t count = getItemsCount();
        for ( uint32_t i = 0; i < count; ++i ) {
            if ( createScope( i ) == false ) {
                return false;
            }

            success = forEachFunctor( *this );
            success &= endScope();
        }

        return success;
    }

    template < CompileTimeString Name, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, const Deserializer& >
    bool readForEach( Callable forEachFunctor ) const {
        if ( createScope( Name.stringView() ) == false ) {
            return false;
        }

        bool success = readForEach( forEachFunctor );
        success &= endScope();
        return success;
    }

    template < typename KeyT, typename ValueT, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, const Deserializer&, const KeyT&, ValueT& >
    bool readForEach( HashMap< KeyT, ValueT >& outMap, Callable forEachFunctor ) const {
        bool success = false;
        uint32_t count = getItemsCount();
        for ( uint32_t i = 0; i < count; ++i ) {
            ValueT mapValue;
            if constexpr ( std::is_integral_v< KeyT > || IsStringId< KeyT > || IsGuid64< KeyT > ) {
                if ( isSupportingIntegralScopes() ) {
                    if ( createScope( i ) == false ) {
                        return false;
                    }

                    auto scopeKey = getScopeKeyUnderlyingType< KeyT >();
                    getScopeIdentifier( scopeKey );
                    KeyT mapKey( scopeKey );

                    success = forEachFunctor( *this, mapKey, mapValue );
                    success &= endScope();

                    if ( success == false ) {
                        return false;
                    }

                    outMap[ mapKey ] = std::move( mapValue );
                    continue;
                }
            }

            if ( createScope( i ) == false ) {
                return false;
            }

            // convert from stringview to key type
            StringView keyString;
            getScopeIdentifier( keyString );
            KeyT mapKey = stringToScopeKey< KeyT >( keyString );

            success = forEachFunctor( *this, mapKey, mapValue );
            success &= endScope();
            if ( success == false ) {
                return false;
            }

            outMap[ mapKey ] = std::move( mapValue );
        }

        return success;
    }

    template < CompileTimeString Name, typename KeyT, typename ValueT, typename Callable >
    requires std::is_invocable_r_v< bool, Callable, const Deserializer&, const KeyT&, ValueT& >
    bool readForEach( HashMap< KeyT, ValueT >& outMap, Callable forEachFunctor ) const {
        if ( createScope( Name.stringView() ) == false ) {
            return false;
        }

        bool success = readForEach( outMap, forEachFunctor );
        success &= endScope();
        return success;
    }

    template < typename T >
    bool read( StringView name, DynamicArray< T >& outValue ) const {
        if ( createScope( name ) == false ) {
            return false;
        }

        bool success = read( outValue );
        success &= endScope();
        return success;
    }

    template < typename T, uint8_t Size >
    bool read( StringView name, InplaceArray< T, Size >& outValue ) const {
        if ( createScope( name ) == false ) {
            return false;
        }

        bool success = read( outValue );
        success &= endScope();
        return success;
    }

    template < typename T, uint8_t Size >
    bool read( InplaceArray< T, Size >& outValue ) const {
        bool success = true;
        uint8_t count = numericCast< uint8_t >( getItemsCount() );
        for ( uint8_t i = 0; i < count; ++i ) {
            T& arrayValue = outValue.emplace();

            success &= createScope( static_cast< uint32_t >( i ) );
            if ( success == false ) {
                break;
            }

            success = read( arrayValue );
            success &= endScope();

            if ( success == false ) {
                break;
            }
        }

        return success;
    }

    template < typename KeyT, typename ValueT, typename... Args >
    bool read( HashMap< KeyT, ValueT >& out, Args&&... additionalArgs ) const {
        bool success = false;
        uint32_t count = getItemsCount();
        for ( uint32_t i = 0; i < count; ++i ) {
            ValueT mapValue;
            if constexpr ( std::is_integral_v< KeyT > || IsStringId< KeyT > || IsGuid64< KeyT > ) {
                if ( isSupportingIntegralScopes() ) {
                    if ( createScope( i ) == false ) {
                        return false;
                    }

                    auto scopeKey = getScopeKeyUnderlyingType< KeyT >();

                    getScopeIdentifier( scopeKey );
                    success = Deserializer::template read< ValueT, Args... >(
                        mapValue,
                        std::forward< Args... >( additionalArgs )... );
                    success &= endScope();

                    if ( success == false ) {
                        return false;
                    }

                    out[ KeyT( scopeKey ) ] = std::move( mapValue );
                    continue;
                }
            }

            if ( createScope( i ) == false ) {
                return false;
            }

            StringView keyString;
            getScopeIdentifier( keyString );
            KeyT mapKey = stringToScopeKey< KeyT >( keyString );

            success = Deserializer::template read< ValueT, Args... >( mapValue,
                                                                      std::forward< Args... >( additionalArgs )... );
            success &= endScope();

            if ( success == false ) {
                return false;
            }

            out[ mapKey ] = std::move( mapValue );
        }

        return success;
    }

  private:
    virtual bool doRead( bool& outValue ) const = 0;
    virtual bool doRead( StringView name, bool& outValue ) const = 0;

    virtual bool doRead( int8_t& outValue ) const = 0;
    virtual bool doRead( int16_t& outValue ) const = 0;
    virtual bool doRead( int32_t& outValue ) const = 0;
    virtual bool doRead( int64_t& outValue ) const = 0;
    virtual bool doRead( uint8_t& outValue ) const = 0;
    virtual bool doRead( uint16_t& outValue ) const = 0;
    virtual bool doRead( uint32_t& outValue ) const = 0;
    virtual bool doRead( uint64_t& outValue ) const = 0;
    virtual bool doRead( int8_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( int16_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( int32_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( int64_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( uint8_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( uint16_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( uint32_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( uint64_t& outValue, uint8_t base ) const = 0;

    virtual bool doRead( StringView name, int8_t& outValue ) const = 0;
    virtual bool doRead( StringView name, int16_t& outValue ) const = 0;
    virtual bool doRead( StringView name, int32_t& outValue ) const = 0;
    virtual bool doRead( StringView name, int64_t& outValue ) const = 0;
    virtual bool doRead( StringView name, uint8_t& outValue ) const = 0;
    virtual bool doRead( StringView name, uint16_t& outValue ) const = 0;
    virtual bool doRead( StringView name, uint32_t& outValue ) const = 0;
    virtual bool doRead( StringView name, uint64_t& outValue ) const = 0;
    virtual bool doRead( StringView name, int8_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, int16_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, int32_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, int64_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, uint16_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, uint8_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, uint32_t& outValue, uint8_t base ) const = 0;
    virtual bool doRead( StringView name, uint64_t& outValue, uint8_t base ) const = 0;

    virtual bool doRead( float32& outValue ) const = 0;
    virtual bool doRead( float64& outValue ) const = 0;
    virtual bool doRead( StringView name, float32& outValue ) const = 0;
    virtual bool doRead( StringView name, float64& outValue ) const = 0;

    virtual bool doRead( StringView& outValue ) const = 0;
    virtual bool doRead( StringView name, StringView& outValue ) const = 0;

    ONYX_NO_DISCARD virtual bool createScope( uint32_t index ) const = 0;
    ONYX_NO_DISCARD virtual bool createScope( uint64_t index ) const = 0;
    ONYX_NO_DISCARD virtual bool createScope( StringView name ) const = 0;

    ONYX_NO_DISCARD virtual bool endScope() const = 0;

    ONYX_NO_DISCARD virtual uint32_t getItemsCount() const = 0;

    virtual bool getScopeIdentifier( uint32_t& outKey ) const = 0;
    virtual bool getScopeIdentifier( uint64_t& outKey ) const = 0;
    virtual bool getScopeIdentifier( Guid64& outKey ) const = 0;
    virtual bool getScopeIdentifier( StringView& outKey ) const = 0;

    ONYX_NO_DISCARD virtual bool isSupportingIntegralScopes() const = 0;

    template < typename T >
    auto getScopeKeyUnderlyingType() const -> decltype( auto ) {
        if constexpr ( IsStringId< T > ) {
            return T::Invalid;
        } else if constexpr ( IsGuid64< T > ) {
            return uint64_t{ 0 };
        } else if constexpr ( std::is_integral_v< T > ) {
            return T{ 0 };
        } else {
            return StringView{};
        }
    }

    template < typename T >
    auto stringToScopeKey( StringView keyString ) const {
        if constexpr ( std::is_integral_v< T > || IsStringId< T > || IsGuid64< T > ) {
            if constexpr ( IsStringId< T > ) {
                typename T::IdType id = 0;
                std::ignore = std::from_chars( keyString.data(), keyString.data() + keyString.size(), id, 16 );
                return T{ id };
            } else if constexpr ( IsGuid64< T > ) {
                uint64_t id = 0;
                std::ignore = std::from_chars( keyString.data(), keyString.data() + keyString.size(), id, 16 );
                return T{ id };
            } else {
                T key{};
                std::ignore = std::from_chars( keyString.data(), keyString.data() + keyString.size(), key, 10 );
                return key;
            }
        } else {
            return keyString;
        }
    }
};
} // namespace onyx