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
        void MonitorDirectory(const FileSystem::Filepath& path);

        void OnFileChanged(const FileSystem::Filepath& path, FileSystem::FileWatcher::FileAction action);

    private:
        AssetSystem* m_AssetSystem = nullptr;
        FileSystem::FileWatcher m_DirectoryWatcher;
    };
}
