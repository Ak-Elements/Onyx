#pragma once

#include <nlohmann/json.hpp>
#include <onyx/serialize/serializer.h>

namespace onyx::file_system {
class JsonSerializer : public Serializer {
  public:
    JsonSerializer();

  private:
    template < typename T >
    bool doGenericWrite( T outValue );

    template < std::integral T >
    bool doGenericWrite( T outValue, uint8_t base );

    template < typename T >
    bool doGenericWrite( StringView name, T outValue );

    template < std::integral T >
    bool doGenericWrite( StringView name, T outValue, uint8_t base );

    nlohmann::ordered_json& getCurrent();
    nlohmann::ordered_json& getCurrent() const;

    bool doWrite( bool value ) override;

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

    bool doWrite( float32 value ) override;
    bool doWrite( float64 value ) override;

    bool doWrite( StringView value ) override;

    bool doWrite( Vector2s8 value ) override;
    bool doWrite( Vector2s16 value ) override;
    bool doWrite( Vector2s32 value ) override;
    bool doWrite( Vector2s64 value ) override;
    bool doWrite( Vector2u8 value ) override;
    bool doWrite( Vector2u16 value ) override;
    bool doWrite( Vector2u64 value ) override;
    bool doWrite( Vector2u32 value ) override;
    bool doWrite( Vector2f64 value ) override;
    bool doWrite( Vector2f32 value ) override;

    bool doWrite( Vector3s8 value ) override;
    bool doWrite( Vector3s16 value ) override;
    bool doWrite( Vector3s32 value ) override;
    bool doWrite( Vector3s64 value ) override;
    bool doWrite( Vector3u8 value ) override;
    bool doWrite( Vector3u16 value ) override;
    bool doWrite( Vector3u32 value ) override;
    bool doWrite( Vector3u64 value ) override;
    bool doWrite( Vector3f32 value ) override;
    bool doWrite( Vector3f64 value ) override;

    bool doWrite( Vector4s8 value ) override;
    bool doWrite( Vector4s16 value ) override;
    bool doWrite( Vector4s32 value ) override;
    bool doWrite( Vector4s64 value ) override;
    bool doWrite( Vector4u8 value ) override;
    bool doWrite( Vector4u16 value ) override;
    bool doWrite( Vector4u32 value ) override;
    bool doWrite( Vector4u64 value ) override;
    bool doWrite( Vector4f32 value ) override;
    bool doWrite( Vector4f64 value ) override;

    bool createScope( uint32_t index ) override;
    bool createScope( uint64_t index ) override;
    bool createScope( StringView name ) override;

    bool endScope() override;

    bool writeItemsCount( uint8_t /*count*/ ) override { return true; }
    bool writeItemsCount( uint16_t /*count*/ ) override { return true; }
    bool writeItemsCount( uint32_t /*count*/ ) override { return true; }
    bool writeItemsCount( uint64_t /*count*/ ) override { return true; }

    bool isSupportingIntegralScopes() const override { return false; }

  public:
    nlohmann::ordered_json JsonRoot;

  private:
    mutable Stack< nlohmann::ordered_json* > JsonNodes;
    mutable StringView m_CurrentScopeName;
};
} // namespace onyx::file_system
