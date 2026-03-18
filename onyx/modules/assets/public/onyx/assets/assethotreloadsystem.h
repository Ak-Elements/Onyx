#pragma once

#include <onyx/noncopyable.h>
#include <onyx/filesystem/filewatcher.h>

namespace onyx::assets
{
    class AssetSystem;
}

namespace onyx::assets
{
    class AssetHotReloadSystem : public NonCopyable
    {
    public:
        AssetHotReloadSystem(AssetSystem& assetSystem);
        void MonitorDirectory(const FilePath& path);

        void OnFileChanged(const FilePath& path, file_system::FileWatcher::FileAction action);

    private:
        AssetSystem* m_AssetSystem = nullptr;
        file_system::FileWatcher m_DirectoryWatcher;
    };
}
