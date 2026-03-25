#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <onyx/serialize/deserializer.h>

namespace onyx::file_system {
class JsonDeserializer : public Deserializer {
  public:
    JsonDeserializer();
    JsonDeserializer( const nlohmann::ordered_json& json );

  private:
    template < typename T >
    bool DoGenericRead( T& outValue ) const;

    template < std::integral T >
    bool DoGenericRead( T& outValue, uint8_t base ) const;

    template < typename T >
    bool DoGenericRead( StringView name, T& outValue ) const;

    template < std::integral T >
    bool DoGenericRead( StringView name, T& outValue, uint8_t base ) const;

    nlohmann::ordered_json& GetCurrent();
    nlohmann::ordered_json& GetCurrent() const;

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
    bool doRead( float64& outValue ) const override;
    bool doRead( StringView name, float32& outValue ) const override;
    bool doRead( StringView name, float64& outValue ) const override;

    bool doRead( StringView& outValue ) const override;
    bool doRead( StringView name, StringView& outValue ) const override;

    bool createScope( uint32_t index ) const override;
    bool createScope( uint64_t index ) const override;
    bool createScope( StringView name ) const override;
    bool endScope() const override;

    uint32_t getItemsCount() const override;

    bool getScopeIdentifier( uint32_t& outKey ) const override;
    bool getScopeIdentifier( uint64_t& outKey ) const override;
    bool getScopeIdentifier( Guid64& outKey ) const override;
    bool getScopeIdentifier( StringView& outKey ) const override;

    bool isSupportingIntegralScopes() const override { return false; }

  public:
    nlohmann::ordered_json JsonRoot;

  private:
    mutable Stack< nlohmann::ordered_json* > JsonNodes;
    mutable StringView m_CurrentScopeName;
};
} // namespace onyx::file_system
