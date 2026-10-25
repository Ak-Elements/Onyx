#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

#include <onyx/assets/assetformat.h>
#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>
#include <onyx/assets/assetinterface.h>

namespace onyx::assets {
struct AssetLoadRequest;

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
    void onSaveFinished( AssetId id, bool success ) const override {
        Reference< AssetT > ref( this );
        m_savedSignal.dispatch( AssetHandle< AssetT >( id, ref ), success );
    }
#endif

  private:
    LoadedSignalT m_loadedSignal;
#if ONYX_IS_EDITOR

    SavedSignalT m_savedSignal;
#endif
};

} // namespace onyx::assets
