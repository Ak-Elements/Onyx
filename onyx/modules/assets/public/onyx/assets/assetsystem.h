#pragma once

#include <onyx/engine/enginesystem.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetloader.h>

#include <onyx/filesystem/path.h>
#include <onyx/log/logger.h>

namespace onyx {
class IEngine;
}

namespace onyx::assets {
struct IAssetSerializer;

struct AssetId;

template < typename T >
concept HasStaticCreate = requires( T obj, IEngine& engine ) { T::create( engine ); };

class AssetSystem : public IEngineSystem {
  public:
    static constexpr StringId32 TypeId{ "onyx::assets::AssetSystem" };
    StringId32 getTypeId() const override { return TypeId; }

    explicit AssetSystem( IEngine& engine );
    ~AssetSystem() override;

    template < typename T >
    AssetHandle< T > create() {
        const InplaceFunction< Reference< AssetInterface >( IEngine& ) >& createFunctor = s_registeredAssets.at(
            T::TypeId );
        AssetHandle< T > newAsset( AssetId::invalid(), createFunctor( *m_engine ) );
        newAsset->setState( AssetState::Loaded );
        return newAsset;
    }

    template < typename T >
    requires std::is_base_of_v< AssetInterface, T >
    static DynamicArray< StringView > getExtensions() {
        constexpr AssetType Type = static_cast< AssetType >( T::TypeId.getId() );
        return getExtensions( Type );
    }

    static DynamicArray< StringView > getExtensions( AssetType type ) {
        DynamicArray< StringView > extensions;
        for( auto&& [ extension, assetType ] : s_extensionToAssetType ) {
            if( assetType == type ) {
                extensions.emplace_back( extension );
            }
        }
        return extensions;
    }

    auto getAssetTypes() { return s_registeredAssets | std::views::keys; }

    Optional< const AssetMetaData* > tryGetAssetMeta( AssetId id ) const {
        auto it = m_assetsMetaData.find( id );
        if( it == m_assetsMetaData.end() )
            return std::nullopt;

        return &it->second;
    }

    const AssetMetaData& getAssetMeta( AssetId id ) const { return m_assetsMetaData.at( id ); }

    DynamicArray< AssetMetaData > getAvailableAssets( AssetType type ) const {
        DynamicArray< AssetMetaData > availableAssets;
        availableAssets.reserve( m_assetsMetaData.size() );

        for( const AssetMetaData& assetMeta : ( m_assetsMetaData | std::views::values ) ) {
            String extension( assetMeta.getExtension() );
            if( ( type != AssetType::Invalid ) && ( s_extensionToAssetType[ extension ] != type ) ) {
                continue;
            }

            availableAssets.push_back( assetMeta );
        }

        return availableAssets;
    }

    template < typename T > requires std::is_base_of_v< AssetInterface, T >
    DynamicArray< AssetMetaData > getAvailableAssets() {
        constexpr AssetType Type = static_cast< AssetType >( T::TypeId.GetId() );

        DynamicArray< AssetMetaData > availableAssets;
        availableAssets.reserve( m_assetsMetaData.size() );

        for( const AssetMetaData& assetMeta : ( m_assetsMetaData | std::views::values ) ) {
            if( assetMeta.Type == Type ) {
                availableAssets.push_back( assetMeta );
            }
        }

        return availableAssets;
    }

    template < typename T, uint64_t N >
    constexpr bool getAsset( const CompileTimeString< N >& path, AssetHandle< T >& outAsset ) {
        AssetId id = static_cast< AssetId >( hash::fnV1aHash( path, 0 ) );
        return getAsset( id, outAsset, false );
    }

    template < typename T >
    bool getAsset( AssetId id, AssetHandle< T >& outAsset ) {
        return getAsset( id, outAsset, false );
    }

    template < typename T >
    bool getAsset( AssetId id, AssetHandle< T >& outAsset, bool forceLoad );

    template < typename T >
    bool getAsset( AssetId id, AssetHandle< T >& outAssetReference ) const;

#if ONYX_IS_EDITOR
    // Used to load copies of assets for working copies in editors (e.g.: Scene / Nodegraph)
    template < typename T >
    bool getAssetUnmanaged( AssetId id, AssetHandle< T >& outAssetReference );

    template < typename T >
    bool saveAsset( const AssetHandle< T >& asset );

    template < typename T >
    bool saveAssetAs( const FilePath& newPath, const AssetHandle< T >& asset );
#endif

    template < typename AssetT > requires std::is_base_of_v< AssetInterface, AssetT >
    static constexpr bool registerAsset();

    template < typename SerializerT, typename... Args >
    static constexpr bool registerSerializer( Args&&... args ) {
        ONYX_ASSERT( s_registeredSerializer.contains( SerializerT::AssetT::TypeId ) == false,
                     "Serializer with that type is already registered." );
        s_registeredSerializer[ SerializerT::AssetT::TypeId ] = makeUnique< SerializerT >(
            std::forward< Args >( args )... );

        for( StringView extension : SerializerT::Extensions ) {
            s_extensionToAssetType[ extension ] = static_cast< AssetType >( SerializerT::AssetT::TypeId.getId() );
        }

        return true;
    }

    void reloadAsset( AssetId id );

  private:
    std::mutex m_mutex;
    AssetIOHandler m_ioHandler;

    HashMap< AssetId, AssetMetaData > m_assetsMetaData;
    DynamicArray< AssetHandle< AssetInterface > > m_loadedAssets;

    static HashMap< StringId32, InplaceFunction< Reference< AssetInterface >( IEngine& ) > > s_registeredAssets;
    static HashMap< StringId32, UniquePtr< IAssetSerializer > > s_registeredSerializer;
    static HashMap< StringView, AssetType > s_extensionToAssetType;

    IEngine* m_engine = nullptr;
};
template < typename AssetT >
requires std::is_base_of_v< AssetInterface, AssetT >
inline constexpr bool AssetSystem::registerAsset() {
    ONYX_ASSERT( s_registeredAssets.contains( AssetT::TypeId ) == false,
                 "Asset with that type is already registered." );
    if constexpr( (std::is_abstract_v< AssetT >) || HasStaticCreate< AssetT > ) {
        s_registeredAssets[ AssetT::TypeId ] = []( IEngine& engine ) -> Reference< AssetInterface > {
            return AssetT::create( engine );
        };
    } else {
        s_registeredAssets[ AssetT::TypeId ] = []( IEngine& ) -> Reference< AssetInterface > {
            return Reference< AssetT >::create();
        };
    }

    return true;
}

template < typename T >
bool AssetSystem::getAsset( AssetId id, AssetHandle< T >& outAsset, bool forceLoad ) {
    auto assetIt = m_assetsMetaData.find( id );
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    if( assetIt == m_assetsMetaData.end() ) {
        ONYX_LOG_ERROR( "Missing asset with id:{}.", id.get() );
        return false;
    }
#endif

    AssetMetaData& metaData = assetIt->second;
    if( ( metaData.Handle != InvalidIndex64 ) && ( forceLoad == false ) ) {
        outAsset = m_loadedAssets[ metaData.Handle ];
        return true;
    }

    constexpr StringId32 AssetTypeHash = T::TypeId;
    metaData.Type = static_cast< AssetType >( AssetTypeHash.getId() );

    if constexpr( HasAssetFormat< T > )
        metaData.Format = T::Format;

    if( ( metaData.Handle != InvalidIndex64 ) && forceLoad ) {
        AssetHandle< AssetInterface >& reloadAsset = m_loadedAssets[ metaData.Handle ];
        {
            std::lock_guard lock( m_mutex );
            const UniquePtr< IAssetSerializer >& serializer = s_registeredSerializer.at( AssetTypeHash );
            m_ioHandler.requestLoad( metaData, reloadAsset, serializer, m_engine );
        }

        outAsset = reloadAsset;
        return true;
    }

    const InplaceFunction< Reference< AssetInterface >( IEngine& ) >& createFunctor = s_registeredAssets.at(
        AssetTypeHash );
    const UniquePtr< IAssetSerializer >& serializer = s_registeredSerializer.at( AssetTypeHash );

    Reference< AssetInterface > newAsset = createFunctor( *m_engine );
    newAsset->setState( AssetState::Loading );

    AssetHandle< AssetInterface > newAssetHandle = { id, newAsset };
    outAsset = newAssetHandle;

    {
        std::lock_guard lock( m_mutex );
        if( metaData.Handle == InvalidIndex64 ) {
            metaData.Handle = static_cast< int64_t >( m_loadedAssets.size() );
            m_loadedAssets.emplace_back( id, std::move( newAsset ) );
            m_ioHandler.requestLoad( metaData, newAssetHandle, serializer, m_engine );
        }
    }

    return true;
}

template < typename T >
bool AssetSystem::getAsset( AssetId id, AssetHandle< T >& outAssetReference ) const {
    const auto assetIt = m_assetsMetaData.find( id );
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    if( assetIt == m_assetsMetaData.end() ) {
        ONYX_LOG_ERROR( "Missing asset with id:{}.", id.get() );
        return false;
    }
#endif

    const AssetMetaData& metaData = assetIt->second;
    if( metaData.Handle == InvalidIndex64 ) {
        return false;
    }

    outAssetReference = m_loadedAssets[ metaData.Handle ];
    return true;
}

#if ONYX_IS_EDITOR
template < typename T >
bool AssetSystem::getAssetUnmanaged( AssetId id, AssetHandle< T >& outAssetReference ) {
    const auto assetIt = m_assetsMetaData.find( id );

    if( assetIt == m_assetsMetaData.end() ) {
        ONYX_LOG_ERROR( "Missing asset with id:{}.", id.get() );
        return false;
    }

    const AssetMetaData& metaData = assetIt->second;

    constexpr StringId32 AssetTypeHash = T::TypeId;
    const InplaceFunction< Reference< AssetInterface >( IEngine& ) >& createFunctor = s_registeredAssets.at(
        AssetTypeHash );
    const UniquePtr< IAssetSerializer >& serializer = s_registeredSerializer.at( AssetTypeHash );

    Reference< AssetInterface > assetCopy = createFunctor( *m_engine );
    assetCopy->setState( AssetState::Loading );

    assets::AssetHandle newAssetHandle = { id, assetCopy };
    outAssetReference = newAssetHandle;

    m_ioHandler.requestLoad( metaData, newAssetHandle, serializer, m_engine );

    return true;
}

template < typename T >
bool AssetSystem::saveAsset( const AssetHandle< T >& asset ) {
    AssetId assetId = asset.getId();
    const auto assetIt = m_assetsMetaData.find( assetId );

    if( assetIt == m_assetsMetaData.end() ) {
        ONYX_LOG_ERROR( "Missing asset with id:{}.", assetId.get() );
        return false;
    }

    const AssetMetaData& metaData = assetIt->second;
    if( metaData.Handle == InvalidIndex64 ) {
        return false;
    }

    constexpr StringId32 AssetTypeHash = T::TypeId;
    const UniquePtr< IAssetSerializer >& serializer = s_registeredSerializer.at( AssetTypeHash );
    const AssetHandle< AssetInterface > assetToSave{ asset };
    m_ioHandler.requestSave( metaData, assetToSave, serializer, m_engine );
    return true;
}

template < typename T >
bool AssetSystem::saveAssetAs( const FilePath& newPath, const AssetHandle< T >& asset ) {
    constexpr StringId32 AssetTypeHash = T::TypeId;
    AssetId newAssetId( newPath );
    AssetMetaData metaData{ .Path = newPath,
                            .Id = newAssetId,
                            .Type = static_cast< AssetType >( AssetTypeHash.getId() ) };
    const UniquePtr< IAssetSerializer >& serializer = s_registeredSerializer.at( AssetTypeHash );

    // TODO: should only add it IF we finish saving
    m_assetsMetaData.try_emplace( newAssetId, metaData );

    const AssetHandle< AssetInterface > assetToSave{ asset };
    m_ioHandler.requestSave( metaData, assetToSave, serializer, m_engine );
    return true;
}
#endif
} // namespace onyx::assets
