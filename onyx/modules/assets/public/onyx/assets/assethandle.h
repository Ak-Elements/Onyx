#pragma once

#include <onyx/assets/assetid.h>
#include <onyx/assets/assetinterface.h>

#include <onyx/assets/assetmetadata.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::assets {
template < typename T > class AssetHandle {
    template < typename U > friend class AssetHandle;

  public:
    AssetHandle() = default;
    AssetHandle( AssetId id, Reference< T > handle )
        : m_id( id )
        , m_handle( handle ) {}

    AssetHandle( const AssetHandle& other )
        : m_id( other.m_id )
        , m_handle( other.m_handle ) {}

    template < typename U >
    requires std::is_base_of_v< T, U > || std::is_base_of_v< U, T >
    explicit AssetHandle( const AssetHandle< U >& other )
        : m_id( other.m_id )
        , m_handle( other.m_handle ) {}

    AssetHandle( AssetHandle&& other ) noexcept
        : m_id( other.m_id )
        , m_handle( std::move( other.m_handle ) ) {}

    template < typename U >
    requires std::is_base_of_v< T, U > || std::is_base_of_v< U, T >
    explicit AssetHandle( AssetHandle< U >&& other ) noexcept
        : m_id( other.m_id )
        , m_handle( std::move( other.m_handle ) ) {}

    AssetHandle& operator=( const AssetHandle& other ) {
        if( this == &other )
            return *this;

        m_id = other.m_id;
        m_handle = other.m_handle;
        return *this;
    }

    template < typename U >
    requires std::is_base_of_v< T, U > || std::is_base_of_v< U, T >
    AssetHandle& operator=( const AssetHandle< U >& other ) {
        m_id = other.m_id;
        m_handle = other.m_handle;
        return *this;
    }

    template < typename U >
    requires std::is_base_of_v< T, U > || std::is_base_of_v< U, T >
    AssetHandle& operator=( AssetHandle< U >&& other ) noexcept {
        m_id = other.m_id;
        m_handle = std::move( other.m_handle );
        return *this;
    }

    AssetHandle& operator=( AssetHandle&& other ) noexcept {
        if( this == &other )
            return *this;

        m_id = other.m_id;
        m_handle = std::move( other.m_handle );
        return *this;
    }

    bool operator==( const AssetHandle& other ) const { return m_id == other.m_id; }
    bool operator!=( const AssetHandle& other ) const { return m_id != other.m_id; }

    [[nodiscard]] bool hasAssetId() const { return m_id.isValid(); }
    [[nodiscard]] bool isValid() const { return m_handle.isValid(); }
    [[nodiscard]] bool hasHandle() const { return m_handle != Reference< T >::invalid(); }

    [[nodiscard]] bool isLoading() const { return isValid() && m_handle->isLoading(); }
    [[nodiscard]] bool isLoaded() const { return isValid() && m_handle->isLoaded(); }

    void reset() {
        m_id = AssetId::invalid();
        m_handle.reset();
    }

    template < typename U >
    requires std::is_base_of_v< T, U > || std::is_base_of_v< U, T >
    U& as() {
        return m_handle.template as< U >();
    }

    template < typename U >
    requires std::is_base_of_v< T, U > || std::is_base_of_v< U, T >
    const U& as() const {
        return m_handle.template as< U >();
    }

    T* operator->() { return m_handle.raw(); }
    const T* operator->() const { return m_handle.raw(); }

    T& operator*() { return *m_handle; }
    const T& operator*() const { return *m_handle; }

    // AssetId should be returned by value here (as it is the same size as uint64_t in non debug builds
    // the problem is that we currently store the debug string in the assetId so returning a tmp
    // can break the returned StringView - AssetId should implement a similar mechanism to StringId
    [[nodiscard]] const AssetId& getId() const { return m_id; }
    void setId( const AssetId& id ) {
        if( id != m_id ) {
            m_id = id;
            m_handle.reset();
        }
    }

    Reference< T >& getHandle() { return m_handle; }
    const Reference< T >& getHandle() const { return m_handle; }

  private:
    AssetId m_id;
    Reference< T > m_handle;
};
} // namespace onyx::assets

namespace onyx {
template < typename U > struct Serialization< assets::AssetHandle< U > > {
    static bool serialize( Serializer& serializer, const assets::AssetHandle< U >& assetHandle ) {
        return serializer.write< "assetId" >( assetHandle.getId() );
    }
    static bool deserialize( const Deserializer& deserializer, assets::AssetHandle< U >& outAssetHandle ) {
        assets::AssetId assetId;
        if( deserializer.read< "assetId" >( assetId ) ) {
            outAssetHandle.setId( assetId );
            return true;
        }

        return false;
    }
};

template <>
struct EngineVariable< assets::AssetId > : public IEngineVariable {
  public:
    explicit EngineVariable( StringId32 id, assets::AssetType assetType )
        : IEngineVariable( id )
        , m_type( assetType ) {}

    [[nodiscard]] uint32_t getRuntimeTypeId() const override { return typeHash< EngineVariable< assets::AssetId > >(); }

    void set( assets::AssetId value ) { m_value = value; }
    [[nodiscard]] assets::AssetId get() const { return m_value; }

    [[nodiscard]] assets::AssetType getType() const { return m_type; }

    bool serialize( Serializer& serializer ) const override { return serializer.write( m_value ); }
    bool deserialize( const Deserializer& deserializer ) override { return deserializer.read( m_value ); }

  private:
    [[nodiscard]] void* doGet() const override { return std::bit_cast< void* >( &m_value ); }
    void doSet( void* value ) override { m_value = *std::bit_cast< assets::AssetId* >( value ); }

  private:
    assets::AssetId m_value;
    assets::AssetType m_type;
};

} // namespace onyx
