#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <onyx/serialize/deserializer.h>

namespace onyx::file_system {
class JsonDeserializer : public Deserializer {
  public:
    JsonDeserializer();
    JsonDeserializer( StringView json );

  private:
    template < typename T >
    bool doGenericRead( T& outValue ) const;

    template < Numeric T >
    bool doGenericRead( T& outValue, uint8_t base ) const;

    template < typename T >
    bool doGenericRead( StringView name, T& outValue ) const;

    template < Numeric T >
    bool doGenericRead( StringView name, T& outValue, uint8_t base ) const;

    nlohmann::ordered_json& getCurrent();
    nlohmann::ordered_json& getCurrent() const;

    bool getScopeIdentifier( uint32_t& outKey ) const override;
    bool getScopeIdentifier( uint64_t& outKey ) const override;
    bool getScopeIdentifier( Guid64& outKey ) const override;
    bool getScopeIdentifier( StringView& outKey ) const override;

  private:
    // Serializer interface
    bool doRead( bool& outValue ) const override;

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

    bool doRead( float32& outValue ) const override;
    bool doRead( float32& outValue, uint8_t base ) const override;
    bool doRead( float64& outValue ) const override;
    bool doRead( float64& outValue, uint8_t base ) const override;

    bool doRead( StringView& outValue ) const override;

    bool doRead( Guid64& outValue ) const override;

    bool doRead( Vector2s8& outValue ) const override;
    bool doRead( Vector2s16& outvalue ) const override;
    bool doRead( Vector2s32& outValue ) const override;
    bool doRead( Vector2s64& outValue ) const override;
    bool doRead( Vector2u8& outValue ) const override;
    bool doRead( Vector2u16& outValue ) const override;
    bool doRead( Vector2u32& outValue ) const override;
    bool doRead( Vector2u64& outValue ) const override;
    bool doRead( Vector2f32& outValue ) const override;
    bool doRead( Vector2f64& outValue ) const override;

    bool doRead( Vector3s8& outValue ) const override;
    bool doRead( Vector3s16& outValue ) const override;
    bool doRead( Vector3s32& outValue ) const override;
    bool doRead( Vector3s64& outValue ) const override;
    bool doRead( Vector3u8& outValue ) const override;
    bool doRead( Vector3u16& outValue ) const override;
    bool doRead( Vector3u32& outValue ) const override;
    bool doRead( Vector3u64& outValue ) const override;
    bool doRead( Vector3f32& outValue ) const override;
    bool doRead( Vector3f64& outValue ) const override;

    bool doRead( Vector4s8& outValue ) const override;
    bool doRead( Vector4s16& outValue ) const override;
    bool doRead( Vector4s32& outValue ) const override;
    bool doRead( Vector4s64& outValue ) const override;
    bool doRead( Vector4u8& outValue ) const override;
    bool doRead( Vector4u16& outValue ) const override;
    bool doRead( Vector4u32& outValue ) const override;
    bool doRead( Vector4u64& outValue ) const override;
    bool doRead( Vector4f32& outValue ) const override;
    bool doRead( Vector4f64& outValue ) const override;

    bool createScope( uint32_t index ) const override;
    bool createScope( uint64_t index ) const override;
    bool createScope( StringView name ) const override;
    bool endScope() const override;

    uint32_t getItemsCount() const override;

    bool isSupportingIntegralScopes() const override { return false; }

  public:
    nlohmann::ordered_json JsonRoot;

  private:
    mutable Stack< nlohmann::ordered_json* > m_jsonNodes;
    mutable StringView m_currentScopeName;
};
} // namespace onyx::file_system
