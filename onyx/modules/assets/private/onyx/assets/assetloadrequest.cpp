#include <onyx/assets/assetloadrequest.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetserializer.h>
#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/thread.h>

ONYX_PROFILE_CREATE_TAG(AssetSystem, 0xfc6203);

namespace Onyx::Assets
{
    void AssetLoadRequest::Start(Threading::ThreadPool& loaderPool)
    {
        Threading::AsyncTask<void()> loadingTask([this]() { Load(); });
        m_Future = loadingTask.GetFuture();
        m_Future.Then([this]()
        {
            if (OnLoadFinished)
                OnLoadFinished(Handle);
        });

        loaderPool.Post(loadingTask);
    }

    void AssetLoadRequest::Cancel()
    {
        m_Future.Cancel();
    }

    void AssetLoadRequest::Load()
    {
        //ONYX_PROFILE
        ONYX_PROFILE(AssetSystem);
        ONYX_PROFILE_FUNCTION;
        

        // might add other threads here that are not valid for loading (e.g.: Present thread / render thread.. etc.)
        ONYX_ASSERT(Thread::MAIN_THREAD_ID != std::this_thread::get_id(), "Do not load assets on the main thread");

        FileSystem::Filepath relativePath = Path.lexically_relative(FileSystem::Path::GetWorkingDirectory());
        String assetName = relativePath.string();
        ZoneText(assetName.c_str(), assetName.length());

        // those load functions should probably be static
        if (Serializer->DeserializeJson(Handle, Path) == false)
        {
            Handle->SetState(AssetState::Invalid);
        }
        else
        {
            Handle->SetState(AssetState::Loaded);
        }

        Handle->OnLoadFinished();
    }

#if ONYX_IS_EDITOR
    void AssetSaveRequest::Start(Threading::ThreadPool& loaderPool)
    {
        ONYX_PROFILE(AssetSystem);
        
        Threading::AsyncTask<void()> loadingTask([this]() { Save(); });
        m_Future = loadingTask.GetFuture();
        m_Future.Then([this]()
            {
                if (OnSaveFinished)
                    OnSaveFinished(Handle);
            });

        loaderPool.Post(loadingTask);
    }

    void AssetSaveRequest::Cancel()
    {
        m_Future.Cancel();
    }

    void AssetSaveRequest::Save()
    {
        ONYX_PROFILE(AssetSystem);

        // might add other threads here that are not valid for loading (e.g.: Present thread / render thread.. etc.)
        ONYX_ASSERT(Thread::MAIN_THREAD_ID != std::this_thread::get_id(), "Do not save assets on the main thread");

        //FileSystem::Filepath relativePath = Path.lexically_relative(FileSystem::Path::GetDataDirectory());
        //String assetName = relativePath.string();
        //ZoneText(assetName.c_str(), assetName.length());

        // those load functions should probably be static
        bool succeeded = Serializer->SerializeJson(Handle, Path);

        Handle->OnSaveFinished(succeeded);
    }
#endif
}
