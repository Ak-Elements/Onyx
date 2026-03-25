#pragma once

#include <onyx/filesystem/path.h>

namespace efsw
{
    class FileWatcher;
}

namespace onyx::file_system
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

        void addPath(const FilePath& path, bool recursive);

        Callback<void(const FilePath&, FileAction)> onFileChanged;

    private:
        void onFileAction(const FilePath& path, FileAction action);

    private:
        UniquePtr<efsw::FileWatcher> m_watcher;
    };
}
