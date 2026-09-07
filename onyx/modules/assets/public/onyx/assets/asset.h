#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

#include <onyx/assets/assetformat.h>
#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>

namespace onyx::assets {
struct AssetLoadRequest;

enum class AssetState : uint8_t {
    Invalid = 0, // asset is in undefined invalid state
    Loading,
    Loaded,
    Missing, // asset is defined but can't be found
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

    [[nodiscard]] bool isLoading() const { return m_state == AssetState::Loading; }
    [[nodiscard]] bool isLoaded() const { return m_state == AssetState::Loaded; }

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
        if( state == AssetState::Loaded ) {
            Reference< AssetT > ref( this );
            m_loadedSignal.dispatch( AssetHandle< AssetT >( id, ref ) );
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
