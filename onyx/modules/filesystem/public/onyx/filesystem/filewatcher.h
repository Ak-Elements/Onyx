#pragma once

#include <onyx/filesystem/path.h>

namespace efsw
{
    class FileWatcher;
}

namespace Onyx::FileSystem
{
    class FileWatcher
    {
    public:
        enum class FileAction
        {
            Invalid,
            Add,
            Delete,
            Modified,
            Moved
        };

        FileWatcher();
        FileWatcher(const Filepath& path, bool recursive);
        FileWatcher(const Filepath& path);
        ~FileWatcher();

        void AddPath(const Filepath& path, bool recursive);

        Callback<void(const Filepath&, FileAction)> OnFileChanged;

    private:
        void OnFileAction(const Filepath& path, FileAction action);

    private:
        UniquePtr<efsw::FileWatcher> m_Watcher = nullptr;
    };
}
