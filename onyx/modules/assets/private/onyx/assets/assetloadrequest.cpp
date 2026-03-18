#include <onyx/assets/assetloadrequest.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/jsonserializer.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/thread.h>

ONYX_PROFILE_CREATE_TAG(AssetSystem, 0xfc6203);

namespace onyx::assets
{
    void AssetLoadRequest::Start(threading::ThreadPool& loaderPool)
    {
        threading::AsyncTask<void()> loadingTask([this]() { Load(); });
        m_Future = loadingTask.GetFuture();
        m_Future.Then([this]()
        {
            if (OnLoadFinished)
                OnLoadFinished(Asset);
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

        FilePath path = file_system::Path::GetFullPath(MetaData.Path);
        String assetName = MetaData.Path.string();
        
        //tracy_scope_AssetSystem.NameFmt("%s", assetName.c_str());

        bool succeeded = false;
        file_system::OnyxFile assetFile(path);
        switch (MetaData.Format)
        {
            case AssetFormat::Text:
                break;
            case AssetFormat::Binary:
                break;
            case AssetFormat::Json:
            {
                const file_system::JsonValue& inputConfigData = assetFile.LoadJson();
                file_system::JsonDeserializer serializer(inputConfigData.Json);
                succeeded = Serializer->Deserialize(Asset, MetaData, serializer, *Engine);
                break;
            }
        }

        // first trigger loaded callbacks, than set the asset to be valid / loaded
        AssetState state = succeeded ? AssetState::Loaded : AssetState::Invalid;
        Asset->OnLoadFinished(Asset.GetId(), state);
    }

#if ONYX_IS_EDITOR
    void AssetSaveRequest::Start(threading::ThreadPool& loaderPool)
    {
        ONYX_PROFILE(AssetSystem);
        
        threading::AsyncTask<void()> saveTask([this]() { Save(); });
        m_Future = saveTask.GetFuture();
        m_Future.Then([this]()
            {
                if (OnSaveFinished)
                    OnSaveFinished(Asset);
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

        //FilePath relativePath = Path.lexically_relative(file_system::Path::GetDataDirectory());
        //String assetName = relativePath.string();
        //ZoneText(assetName.c_str(), assetName.length());

        file_system::JsonSerializer serializer;
        bool succeeded = Serializer->Serialize(Asset, MetaData, serializer, *Engine);

        const String& jsonString = serializer.JsonRoot.dump(4);
        using namespace file_system;
        OnyxFile inputConfigFile(Path::GetFullPath(MetaData.Path));
        FileStream stream = inputConfigFile.OpenStream(OpenMode::Write | OpenMode::Text);
        stream.WriteRaw(jsonString.data(), jsonString.size());

        Asset->OnSaveFinished(Asset.GetId(), succeeded);
    }
#endif
}
