#include <onyx/assets/assetloadrequest.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/jsonserializer.h>
#include <onyx/filesystem/onyxfile.h>
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

        loaderPool.Post(std::move(loadingTask));
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

        FileSystem::Filepath path = FileSystem::Path::GetFullPath(MetaData.Path);
        String assetName = MetaData.Path.string();
        
        //tracy_scope_AssetSystem.NameFmt("%s", assetName.c_str());

        bool succeeded = false;
        FileSystem::OnyxFile assetFile(path);
        switch (MetaData.Format)
        {
            case AssetFormat::Text:
                break;
            case AssetFormat::Binary:
                break;
            case AssetFormat::Json:
            {
                const FileSystem::JsonValue& inputConfigData = assetFile.LoadJson();
                FileSystem::JsonDeserializer serializer(inputConfigData.Json);
                succeeded = Serializer->Deserialize(Handle, MetaData, serializer);
                break;
            }
        }

        // first trigger loaded callbacks, than set the asset to be valid / loaded
        AssetState state = succeeded ? AssetState::Loaded : AssetState::Invalid;
        Handle->OnLoadFinished(state);
        Handle->SetState(state);
    }

#if ONYX_IS_EDITOR
    void AssetSaveRequest::Start(Threading::ThreadPool& loaderPool)
    {
        ONYX_PROFILE(AssetSystem);
        
        Threading::AsyncTask<void()> saveTask([this]() { Save(); });
        m_Future = saveTask.GetFuture();
        m_Future.Then([this]()
            {
                if (OnSaveFinished)
                    OnSaveFinished(Handle);
            });

        loaderPool.Post(saveTask);
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

        FileSystem::JsonSerializer serializer;
        bool succeeded = Serializer->Serialize(Handle, MetaData, serializer);

        const String& jsonString = serializer.JsonRoot.dump(4);
        using namespace FileSystem;
        OnyxFile inputConfigFile(Path::GetFullPath(MetaData.Path));
        FileStream stream = inputConfigFile.OpenStream(OpenMode::Write | OpenMode::Text);
        stream.WriteRaw(jsonString.data(), jsonString.size());

        Handle->OnSaveFinished(succeeded);
    }
#endif
}
