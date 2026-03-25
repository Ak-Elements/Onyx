#pragma once

#include <charconv>

#include <onyx/onyx_types.h>
#include <onyx/stream/stream.h>

namespace onyx {
class StringStream : public Stream {
  public:
    StringStream() = default;
    explicit StringStream( String str );

    bool isValid() const override { return m_data.empty(); }
    bool isEof() const override { return m_currentDataPosition >= getLength(); }
    uint64_t getPosition() override { return m_currentDataPosition; }
    uint64_t getPosition() const override { return m_currentDataPosition; }
    void setPosition( uint64_t position ) override { m_currentDataPosition = position; }
    uint64_t getLength() const override { return static_cast< uint64_t >( m_data.size() ); }

    const String& getData() { return m_data; }

    void skipWhitespaces();

    bool readString( StringView& outStrView, const char delimiter );
    bool readString( StringView& outStrView );
    bool readLine( StringView& outStrView );
    bool readStringUntil( StringView& outStrView, const char delimiter );
    bool readConditional( StringView expectedValue );

    template < typename T >
    bool readType( T& outValue ) {
        ONYX_ASSERT( isEof() == false );

        const char* startPos = ( m_data.data() + m_currentDataPosition );
        const std::from_chars_result result = std::from_chars( startPos, startPos + getRemainingLength(), outValue );
        ONYX_ASSERT( result.ec != std::errc::invalid_argument );
        ONYX_ASSERT( result.ec != std::errc::result_out_of_range );

        uint32_t count = static_cast< uint32_t >( result.ptr - startPos );
        skip( count );
        return true;
    }

    template < typename T >
    void writeType( const T& val ) {
        String valStr = to_string( val );
        write( valStr );
    }

  private:
    void doRead( char* destination, uint64_t size ) const override;
    void doWrite( const char* data, uint64_t size ) override;

  private:
    String m_data;
    mutable uint64_t m_currentDataPosition = 0;
};
} // namespace onyx
