#include <onyx/assets/assethotreloadsystem.h>

#include <onyx/assets/assetid.h>
#include <onyx/assets/assetsystem.h>

namespace onyx::assets {
AssetHotReloadSystem::AssetHotReloadSystem( AssetSystem& assetSystem )
    : m_assetSystem( &assetSystem ) {
    m_directoryWatcher.OnFileChanged.connect< &AssetHotReloadSystem::onFileChanged >( this );
}

void AssetHotReloadSystem::monitorDirectory( const FilePath& path ) {
    m_directoryWatcher.addPath( path, true );
}

void AssetHotReloadSystem::onFileChanged( [[maybe_unused]] const FilePath& path,
                                          file_system::FileWatcher::FileAction action ) {
    [[maybe_unused]] auto a = m_assetSystem;
    // TODO: FIX
    //// AssetId assetId( path );

    switch( action ) {
    case file_system::FileWatcher::FileAction::Add:
        break;
    case file_system::FileWatcher::FileAction::Delete:
        break;
    case file_system::FileWatcher::FileAction::Modified:
        // m_assetSystem->reloadAsset( assetId );
        break;
    case file_system::FileWatcher::FileAction::Moved:
        break;
    case file_system::FileWatcher::FileAction::Invalid:
        break;
    }
}
} // namespace onyx::assets
