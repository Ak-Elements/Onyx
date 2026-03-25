#pragma once

#include <onyx/filesystem/filewatcher.h>
#include <onyx/noncopyable.h>

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::assets {
class AssetHotReloadSystem : public NonCopyable {
  public:
    explicit AssetHotReloadSystem( AssetSystem& assetSystem );
    void monitorDirectory( const FilePath& path );

    void onFileChanged( const FilePath& path, file_system::FileWatcher::FileAction action );

  private:
    AssetSystem* m_assetSystem = nullptr;
    file_system::FileWatcher m_directoryWatcher;
};
} // namespace onyx::assets
