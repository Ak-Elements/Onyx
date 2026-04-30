#pragma once

#include <onyx/serialize/serializer.h>

namespace onyx::file_system {
class TextSerializer : public Serializer {
  public:
    TextSerializer();

  private:
    template < typename T >
    bool doGenericWrite( T outValue );

    template < std::integral T >
    bool doGenericWrite( T outValue, uint8_t base );

    template < typename T >
    bool doGenericWrite( StringView name, T outValue );

    template < std::integral T >
    bool doGenericWrite( StringView name, T outValue, uint8_t base );

    bool doWrite( bool value ) override;
    bool doWrite( StringView name, bool value ) override;

    bool doWrite( int8_t value ) override;
    bool doWrite( int16_t value ) override;
    bool doWrite( int32_t value ) override;
    bool doWrite( int64_t value ) override;
    bool doWrite( uint8_t value ) override;
    bool doWrite( uint16_t value ) override;
    bool doWrite( uint32_t value ) override;
    bool doWrite( uint64_t value ) override;
    bool doWrite( int8_t value, uint8_t base ) override;
    bool doWrite( int16_t value, uint8_t base ) override;
    bool doWrite( int32_t value, uint8_t base ) override;
    bool doWrite( int64_t value, uint8_t base ) override;
    bool doWrite( uint8_t value, uint8_t base ) override;
    bool doWrite( uint16_t value, uint8_t base ) override;
    bool doWrite( uint32_t value, uint8_t base ) override;
    bool doWrite( uint64_t value, uint8_t base ) override;

    bool doWrite( StringView name, int8_t value ) override;
    bool doWrite( StringView name, int16_t value ) override;
    bool doWrite( StringView name, int32_t value ) override;
    bool doWrite( StringView name, int64_t value ) override;
    bool doWrite( StringView name, uint8_t value ) override;
    bool doWrite( StringView name, uint16_t value ) override;
    bool doWrite( StringView name, uint32_t value ) override;
    bool doWrite( StringView name, uint64_t value ) override;
    bool doWrite( StringView name, int8_t value, uint8_t base ) override;
    bool doWrite( StringView name, int16_t value, uint8_t base ) override;
    bool doWrite( StringView name, int32_t value, uint8_t base ) override;
    bool doWrite( StringView name, int64_t value, uint8_t base ) override;
    bool doWrite( StringView name, uint8_t value, uint8_t base ) override;
    bool doWrite( StringView name, uint16_t value, uint8_t base ) override;
    bool doWrite( StringView name, uint32_t value, uint8_t base ) override;
    bool doWrite( StringView name, uint64_t value, uint8_t base ) override;

    bool doWrite( float32 value ) override;
    bool doWrite( float64 value ) override;
    bool doWrite( StringView name, float32 value ) override;
    bool doWrite( StringView name, float64 value ) override;

    bool doWrite( StringView value ) override;
    bool doWrite( StringView name, StringView value ) override;

    bool createScope( uint32_t index ) override;
    bool createScope( uint64_t index ) override;
    bool createScope( StringView name ) override;

    bool endScope() override;

    bool writeItemsCount( uint8_t /*count*/ ) override { return true; }
    bool writeItemsCount( uint16_t /*count*/ ) override { return true; }
    bool writeItemsCount( uint32_t /*count*/ ) override { return true; }
    bool writeItemsCount( uint64_t /*count*/ ) override { return true; }

    bool isSupportingIntegralScopes() const override { return false; }
};
} // namespace onyx::file_system
