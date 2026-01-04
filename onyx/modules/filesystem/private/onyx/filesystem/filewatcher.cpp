#include <onyx/filesystem/filewatcher.h>

#include <efsw/efsw.hpp>

namespace Onyx::FileSystem
{
    namespace
    {
        // Inherits from the abstract listener class, and implements the the file action handler
        class FileWatchListener : public efsw::FileWatchListener
        {
        public:
            void handleFileAction(efsw::WatchID /*watchid*/, const std::string& dir, const std::string& filename, efsw::Action action, std::string /*oldFilename*/) override
            {
                FilePath fullpath(dir);
                fullpath.append(filename);
                fullpath = fullpath.lexically_normal();
                FileWatcher::FileAction fileAction = FileWatcher::FileAction::Invalid;
                switch (action)
                {
                    case efsw::Action::Add: fileAction = FileWatcher::FileAction::Add; break;
                    case efsw::Action::Delete: fileAction = FileWatcher::FileAction::Delete; break;
                    case efsw::Action::Modified: fileAction = FileWatcher::FileAction::Modified; break;
                    case efsw::Action::Moved: fileAction = FileWatcher::FileAction::Moved; break;
                }

                if (OnFileChanged)
                    OnFileChanged(fullpath, fileAction);
            }

            Callback<void(const FilePath&, FileWatcher::FileAction)> OnFileChanged;

        };
    }

    FileWatchListener listener;

    FileWatcher::FileWatcher()
        : m_Watcher(MakeUnique<efsw::FileWatcher>())
    {
        listener.OnFileChanged.Connect<&FileWatcher::OnFileAction>(this);
        m_Watcher->watch();
    }

    FileWatcher::FileWatcher(const FilePath& path, bool recursive)
        : FileWatcher()
    {
        m_Watcher->addWatch(path.string(), &listener, recursive);
    }

    FileWatcher::FileWatcher(const FilePath& path)
        : FileWatcher(path, false)
    {
    }

    FileWatcher::~FileWatcher()
    {
        listener.OnFileChanged.Reset();
    }

    void FileWatcher::AddPath(const FilePath& path, bool recursive)
    {
        m_Watcher->addWatch(path.string(), &listener, recursive);
    }

    void FileWatcher::OnFileAction(const FilePath& path, FileAction action)
    {
        if (OnFileChanged)
            OnFileChanged(path, action);
    }
}
