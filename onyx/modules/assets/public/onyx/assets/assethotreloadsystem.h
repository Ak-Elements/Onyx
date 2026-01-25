#pragma once

#include <onyx/noncopyable.h>
#include <onyx/filesystem/filewatcher.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Assets
{
    class AssetHotReloadSystem : public NonCopyable
    {
    public:
        AssetHotReloadSystem(AssetSystem& assetSystem);
        void MonitorDirectory(const FilePath& path);

        void OnFileChanged(const FilePath& path, FileSystem::FileWatcher::FileAction action);

    private:
        AssetSystem* m_AssetSystem = nullptr;
        FileSystem::FileWatcher m_DirectoryWatcher;
    };
}
