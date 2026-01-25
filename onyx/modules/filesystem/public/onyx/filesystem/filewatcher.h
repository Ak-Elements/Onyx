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
        FileWatcher(const FilePath& path, bool recursive);
        FileWatcher(const FilePath& path);
        ~FileWatcher();

        void AddPath(const FilePath& path, bool recursive);

        Callback<void(const FilePath&, FileAction)> OnFileChanged;

    private:
        void OnFileAction(const FilePath& path, FileAction action);

    private:
        UniquePtr<efsw::FileWatcher> m_Watcher;
    };
}
