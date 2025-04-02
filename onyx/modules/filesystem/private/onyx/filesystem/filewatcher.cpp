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
                Filepath fullpath(dir);
                fullpath.append(filename);

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

            Callback<void(const Filepath&, FileWatcher::FileAction)> OnFileChanged;

        };
    }

    FileWatchListener listener;

    FileWatcher::FileWatcher()
        : m_Watcher(MakeUnique<efsw::FileWatcher>())
    {
        listener.OnFileChanged.Connect<&FileWatcher::OnFileAction>(this);
        m_Watcher->watch();
    }

    FileWatcher::FileWatcher(const Filepath& path, bool recursive)
        : FileWatcher()
    {
        m_Watcher->addWatch(path.string(), &listener, recursive);
    }

    FileWatcher::FileWatcher(const Filepath& path)
        : FileWatcher(path, false)
    {
    }

    FileWatcher::~FileWatcher()
    {
        listener.OnFileChanged.Reset();
    }

    void FileWatcher::AddPath(const Filepath& path, bool recursive)
    {
        m_Watcher->addWatch(path.string(), &listener, recursive);
    }

    void FileWatcher::OnFileAction(const Filepath& path, FileAction action)
    {
        if (OnFileChanged)
            OnFileChanged(path, action);
    }
}
