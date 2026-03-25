#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

#include <onyx/assets/assetformat.h>
#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>

namespace onyx::assets {
struct AssetLoadRequest;

// AssetType is the hash of the asset class to use. e.g.: AudioAsset, MeshAsset, StaticMeshAsset, ...
// class hashes are constructed using entt type_info
// NOLINTNEXTLINE
enum class AssetType : uint32_t { Invalid };

enum class AssetState : uint8_t {
    Invalid = 0, // asset is in undefined invalid state
    Loading,
    Loaded,
    Missing, // asset is defined but can't be found
};

struct AssetMetaData {
    FilePath Path;
    AssetId Id;
    AssetType Type = AssetType::Invalid;
    AssetFormat Format = AssetFormat::Json;

    int64_t Handle = InvalidIndex64;

    uint32_t Version = 0; // Maybe not needed

    ONYX_NO_DISCARD String getName() const { return Path.stem().string(); }

    ONYX_NO_DISCARD String getExtension() const {
        // TODO: remove extension once we have meta data stored on disk
        String extension = Path.extension().string();
        if ( extension.empty() == false ) {
            return extension.substr( 1 ); // ignore .
        }

        return "";
    }
};

// Type trait to check if a class has Dependencies using alias
template < typename T, typename = void > struct HasExtension : std::false_type {};

template < typename T > struct HasExtension< T, std::void_t< typename T::Extension > > : std::true_type {};

class AssetInterface : public RefCounted {
    friend struct AssetLoadRequest;
#if ONYX_IS_EDITOR
    friend struct AssetSaveRequest;
#endif
  public:
    void setState( AssetState state ) { m_state = state; }

    bool isLoading() const { return m_state == AssetState::Loading; }
    bool isLoaded() const { return m_state == AssetState::Loaded; }

  private:
    virtual void onLoadFinished( AssetId id, AssetState state ) = 0;
#if ONYX_IS_EDITOR
    virtual void onSaveFinished( AssetId id, bool success ) const = 0;
#endif

  protected:
    Atomic< AssetState > m_state = AssetState::Invalid;
};

template < typename T > class Asset : public AssetInterface {
  public:
    using AssetT = T;

    using LoadedSignalT = Signal< void( AssetHandle< AssetT > ) >;
#if ONYX_IS_EDITOR
    using SavedSignalT = Signal< void( AssetHandle< AssetT >, bool ) >;
#endif

  public:
    Sink< LoadedSignalT > getOnLoadedEvent() { return Sink< LoadedSignalT >( m_loadedSignal ); }
#if ONYX_IS_EDITOR
    Sink< SavedSignalT > getOnSavedEvent() { return Sink< SavedSignalT >( m_savedSignal ); }
#endif

  private:
    void onLoadFinished( AssetId id, AssetState state ) override {
        if ( state == AssetState::Loaded ) {
            Reference< AssetT > ref( this );
            m_loadedSignal.Dispatch( AssetHandle< AssetT >( id, ref ) );
        }

        // set to loaded after callbacks to not trigger code that depends on those callback early
        setState( state );
    }

#if ONYX_IS_EDITOR
    void onSaveFinished( AssetId /*id*/, bool /*success*/ ) const override {
        // Reference<AssetT> ref(this);
        //  m_SavedSignal.Dispatch({ .Id = id, .Handle = ref }, success);
    }
#endif

  private:
    LoadedSignalT m_loadedSignal;
#if ONYX_IS_EDITOR
    SavedSignalT m_savedSignal;
#endif
};

} // namespace onyx::assets
