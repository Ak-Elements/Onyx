#pragma once

#include <onyx/serialize/deserializer.h>

namespace onyx::serialization::text {
namespace internal {
struct Scope;
}

class TextDeserializer : public Deserializer {
  public:
    TextDeserializer();
    TextDeserializer( StringView data );

    ~TextDeserializer() override;

  private:
    template < typename T >
    bool doGenericRead( T& outValue ) const;

    template < typename T >
    bool doGenericRead( StringView name, T& outValue ) const;

    template < Numeric T >
    bool doGenericRead( T& outValue, uint8_t base ) const;

    template < Numeric T >
    bool doGenericRead( StringView name, T& outValue, uint8_t base ) const;

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
    bool doRead( Vector2f64& outValue ) const override;
    bool doRead( Vector2f32& outValue ) const override;

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

    [[nodiscard]] bool createScope( uint32_t index ) const override;
    [[nodiscard]] bool createScope( uint64_t index ) const override;
    [[nodiscard]] bool createScope( StringView name ) const override;
    [[nodiscard]] bool endScope() const override;

    [[nodiscard]] uint32_t getItemsCount() const override;

    [[nodiscard]] bool isSupportingIntegralScopes() const override { return false; }

  private:
    StringView m_data;
    mutable UniquePtr< internal::Scope > m_root;
    mutable Stack< internal::Scope* > m_scopeStack;
};
} // namespace onyx::serialization::text
