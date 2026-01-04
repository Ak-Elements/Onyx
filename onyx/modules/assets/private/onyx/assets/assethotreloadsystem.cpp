#include <onyx/assets/assethotreloadsystem.h>

#include <onyx/assets/assetid.h>
#include <onyx/assets/assetsystem.h>

namespace Onyx::Assets
{
    AssetHotReloadSystem::AssetHotReloadSystem(AssetSystem& assetSystem)
        : m_AssetSystem(&assetSystem)
    {
        m_DirectoryWatcher.OnFileChanged.Connect<&AssetHotReloadSystem::OnFileChanged>(this);
    }

    void AssetHotReloadSystem::MonitorDirectory(const FilePath& path)
    {
        m_DirectoryWatcher.AddPath(path, true);
    }

    void AssetHotReloadSystem::OnFileChanged(const FilePath& path, FileSystem::FileWatcher::FileAction action)
    {
        AssetId assetId(path);

        switch (action)
        {
        case FileSystem::FileWatcher::FileAction::Add:
            break;
        case FileSystem::FileWatcher::FileAction::Delete:
            break;
        case FileSystem::FileWatcher::FileAction::Modified:
            m_AssetSystem->ReloadAsset(assetId);
            break;
        case FileSystem::FileWatcher::FileAction::Moved:
            break;
        case FileSystem::FileWatcher::FileAction::Invalid:
            break;
        }
        

    }
}
