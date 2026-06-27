#pragma once

namespace onyx {
class Stream;

template < typename T >
concept HasSerialize = requires( const T& obj, Stream& outStream ) {
    { obj.serialize( outStream ) } -> std::same_as< void >;
};

template < typename T >
concept HasDeserialize = requires( T& obj, const Stream& inStream ) {
    { obj.deserialize( inStream ) } -> std::same_as< void >;
};

class Stream {
  public:
    virtual ~Stream() = default;

    // interface to implement for streams
    ONYX_NO_DISCARD virtual bool isValid() const = 0;
    ONYX_NO_DISCARD virtual bool isEof() const = 0;

    ONYX_NO_DISCARD virtual uint64_t getPosition() = 0;
    ONYX_NO_DISCARD virtual uint64_t getPosition() const = 0;
    virtual void setPosition( uint64_t position ) = 0;

    ONYX_NO_DISCARD virtual uint64_t getLength() const = 0;

    explicit operator bool() const { return isValid(); }
    ONYX_NO_DISCARD uint64_t getRemainingLength() const {
        ONYX_ASSERT( getLength() >= getPosition() );
        return getLength() - getPosition();
    }

    void reset() { setPosition( 0 ); }

    template < typename T >
    void peek( T& out ) {
        uint64_t position = getPosition();
        read( out );
        setPosition( position );
    }

    void skip() { setPosition( getPosition() + 1 ); }
    void skip( uint32_t count ) { setPosition( getPosition() + count ); }

    // Read Raw interface
    template < typename T >
    void read( T& out ) const {
        if constexpr( HasDeserialize< T > )
            out.deserialize( *this );
        else
            readRaw( out );
    }

    void read( String& outStr ) const;
    void read( String& outStr, uint64_t length ) const;

    template < typename T >
    void read( StringId< T >& outId ) const {
        T id;
        read( id );

#if ONYX_IS_RETAIL
        outId = { id };
#else
        String idString;
        read( idString );

        outId = { id, idString };
#endif
    }

    template < typename T >
    void read( DynamicArray< T >& array, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            array.resize( length );
            doRead( reinterpret_cast< char* >( array.data() ), sizeof( T ) * length );
        }
    }

    template < template < typename > typename Container, typename T >
    void read( Container< T >& array, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            array.resize( length );
            for( uint64_t i = 0; i < length; ++i ) {
                read( array[ i ] );
            }
        }
    }

    template < typename T >
    void read( Set< T >& set, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            for( uint64_t i = 0; i < length; ++i ) {
                T element;
                read( element );
                set.emplace( element );
            }
        }
    }

    template < typename T >
    void read( HashSet< T >& set, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            for( uint64_t i = 0; i < length; ++i ) {
                T element;
                read( element );
                set.emplace( element );
            }
        }
    }

    template < typename KeyT, typename ValueT >
    void read( HashMap< KeyT, ValueT >& map, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            KeyT key{};
            for( uint64_t i = 0; i < length; ++i ) {
                read( key );
                read( map[ key ] );
            }
        }
    }

    void readRaw( char* data, uint64_t size ) { doRead( data, size ); }

    // Read Raw (binary data)
    template < typename T >
    void readRaw( T& out ) const {
        doRead( reinterpret_cast< char* >( &out ), sizeof( T ) );
    }

    template < typename T >
    void readRaw( DynamicArray< T >& array, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            array.resize( length );
            for( uint64_t i = 0; i < length; ++i ) {
                readRaw( array[ i ] );
            }
        }
    }

    template < typename T, uint8_t Size >
    void readRaw( InplaceArray< T, Size >& array, uint64_t length = 0 ) {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            for( uint64_t i = 0; i < length; ++i ) {
                readRaw( array[ i ] );
            }
        }
    }

    template < typename KeyT, typename ValueT >
    void readRaw( HashMap< KeyT, ValueT >& map, uint64_t length = 0 ) const {
        if( length == 0 )
            read( length );

        if( length != 0 ) {
            // map.reserve(length);
            KeyT key;
            for( uint64_t i = 0; i < length; ++i ) {
                readRaw( key );
                readRaw( map[ key ] );
            }
        }
    }

    // Write interface
    template < typename T >
    void write( const T& val ) {
        if constexpr( HasSerialize< T > )
            val.serialize( *this );
        else
            writeRaw( val );
    }

    void write( const String& val );
    void write( StringView val );

    template < typename T >
    void write( StringId< T > id ) {
        write( id.getId() );
#if !ONYX_IS_RETAIL
        write( id.getString() );
#endif
    }

    template < template < typename > typename Container, typename T >
    void write( const Container< T >& array, bool writeSize = true ) {
        if( writeSize )
            write( static_cast< uint64_t >( array.size() ) );

        if( array.empty() == false ) {
            for( const T& element : array ) {
                write( element );
            }
        }
    }

    template < typename T >
    void write( const HashSet< T >& set, bool writeSize = true ) {
        if( writeSize )
            write( static_cast< uint64_t >( set.size() ) );

        if( set.empty() == false ) {
            for( const T& element : set ) {
                write( element );
            }
        }
    }

    template < typename KeyT, typename ValueT >
    void write( const HashMap< KeyT, ValueT >& map, bool writeSize = true ) {
        if( writeSize )
            write( static_cast< uint64_t >( map.size() ) );

        for( const auto& [ key, value ] : map ) {
            write( key );
            write( value );
        }
    }

    void writeRaw( const char* data, uint64_t size ) { doWrite( data, size ); }

    // Write Raw (binary data)
    template < typename T >
    void writeRaw( const T& val ) {
        doWrite( reinterpret_cast< const char* >( &val ), sizeof( T ) );
    }

    template < typename T >
    void writeRaw( const DynamicArray< T >& array, bool writeSize = true ) {
        if( writeSize )
            write( static_cast< uint64_t >( array.size() ) );

        if( array.empty() == false )
            doWrite( reinterpret_cast< const char* >( array.data() ), sizeof( T ) * array.size() );
    }

    template < typename T, uint8_t Size >
    void writeRaw( const InplaceArray< T, Size >& array, bool writeSize = true ) {
        if( writeSize )
            write( static_cast< uint64_t >( array.size() ) );

        if( array.empty() == false )
            doWrite( reinterpret_cast< const char* >( array.data() ), sizeof( T ) * array.size() );
    }

    template < typename KeyT, typename ValueT >
    void writeRaw( const HashMap< KeyT, ValueT >& map, bool writeSize = true ) {
        if( writeSize )
            write( static_cast< int64_t >( map.size() ) );

        for( const auto& [ key, value ] : map ) {
            writeRaw( key );
            writeRaw( value );
        }
    }

  private:
    virtual void doRead( char* destination, uint64_t size ) const = 0;
    virtual void doWrite( const char* data, uint64_t size ) = 0;
};
} // namespace onyx
