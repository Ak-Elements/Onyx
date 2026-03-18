#include <onyx/assets/assethotreloadsystem.h>

#include <onyx/assets/assetid.h>
#include <onyx/assets/assetsystem.h>

namespace onyx::assets
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

    void AssetHotReloadSystem::OnFileChanged(const FilePath& path, file_system::FileWatcher::FileAction action)
    {
        AssetId assetId(path);

        switch (action)
        {
        case file_system::FileWatcher::FileAction::Add:
            break;
        case file_system::FileWatcher::FileAction::Delete:
            break;
        case file_system::FileWatcher::FileAction::Modified:
            m_AssetSystem->ReloadAsset(assetId);
            break;
        case file_system::FileWatcher::FileAction::Moved:
            break;
        case file_system::FileWatcher::FileAction::Invalid:
            break;
        }
        

    }
}
