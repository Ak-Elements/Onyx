#pragma once

#include <onyx/serialize/deserializer.h>

namespace onyx::file_system {
class TextDeserializer : public Deserializer {
  public:
    TextDeserializer();
    TextDeserializer( StringView data );

  private:
    template < typename T >
    bool doGenericRead( T& outValue ) const;

    template < typename T >
    bool doGenericRead( StringView name, T& outValue ) const;

    template < Numeric T >
    bool doGenericRead( T& outValue, uint8_t base ) const;

    template < Numeric T >
    bool doGenericRead( StringView name, T& outValue, uint8_t base ) const;

  private:
    // Serializer interface
    bool doRead( bool& outValue ) const override;
    bool doRead( StringView name, bool& outValue ) const override;

    bool doRead( int8_t& outValue ) const override;
    bool doRead( int16_t& outValue ) const override;
    bool doRead( int32_t& outValue ) const override;
    bool doRead( int64_t& outValue ) const override;
    bool doRead( uint8_t& outValue ) const override;
    bool doRead( uint16_t& outValue ) const override;
    bool doRead( uint32_t& outValue ) const override;
    bool doRead( uint64_t& outValue ) const override;
    bool doRead( int8_t& outValue, uint8_t base ) const override;
    bool doRead( int16_t& outValue, uint8_t base ) const override;
    bool doRead( int32_t& outValue, uint8_t base ) const override;
    bool doRead( int64_t& outValue, uint8_t base ) const override;
    bool doRead( uint8_t& outValue, uint8_t base ) const override;
    bool doRead( uint16_t& outValue, uint8_t base ) const override;
    bool doRead( uint32_t& outValue, uint8_t base ) const override;
    bool doRead( uint64_t& outValue, uint8_t base ) const override;

    bool doRead( StringView name, int8_t& outValue ) const override;
    bool doRead( StringView name, int16_t& outValue ) const override;
    bool doRead( StringView name, int32_t& outValue ) const override;
    bool doRead( StringView name, int64_t& outValue ) const override;
    bool doRead( StringView name, uint8_t& outValue ) const override;
    bool doRead( StringView name, uint16_t& outValue ) const override;
    bool doRead( StringView name, uint32_t& outValue ) const override;
    bool doRead( StringView name, uint64_t& outValue ) const override;
    bool doRead( StringView name, int8_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, int16_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, int32_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, int64_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, uint8_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, uint16_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, uint32_t& outValue, uint8_t base ) const override;
    bool doRead( StringView name, uint64_t& outValue, uint8_t base ) const override;

    bool doRead( float32& outValue ) const override;
    bool doRead( float32& outValue, uint8_t base ) const override;
    bool doRead( float64& outValue ) const override;
    bool doRead( float64& outValue, uint8_t base ) const override;
    bool doRead( StringView name, float32& outValue ) const override;
    bool doRead( StringView name, float32& outValue, uint8_t base ) const override;
    bool doRead( StringView name, float64& outValue ) const override;
    bool doRead( StringView name, float64& outValue, uint8_t base ) const override;

    bool doRead( StringView& outValue ) const override;
    bool doRead( StringView name, StringView& outValue ) const override;

    ONYX_NO_DISCARD bool createScope( uint32_t index ) const override;
    ONYX_NO_DISCARD bool createScope( uint64_t index ) const override;
    ONYX_NO_DISCARD bool createScope( StringView name ) const override;
    ONYX_NO_DISCARD bool endScope() const override;

    uint32_t getItemsCount() const override;

    bool getScopeIdentifier( uint32_t& outKey ) const override;
    bool getScopeIdentifier( uint64_t& outKey ) const override;
    bool getScopeIdentifier( Guid64& outKey ) const override;
    bool getScopeIdentifier( StringView& outKey ) const override;

    bool isSupportingIntegralScopes() const override { return false; }

    void updateScope() const;

  private:
    struct Scope {
        StringView Name;
        StringView Data;
        DynamicArray< Scope > Children;
    };

    mutable Scope m_root;
    mutable Stack< Scope* > m_scopeStack;
};
} // namespace onyx::file_system
