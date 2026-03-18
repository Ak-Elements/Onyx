#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

#include <onyx/assets/assetid.h>
#include <onyx/assets/assetformat.h>
#include <onyx/assets/assethandle.h>

namespace onyx::assets
{
    struct AssetLoadRequest;

    // AssetType is the hash of the asset class to use. e.g.: AudioAsset, MeshAsset, StaticMeshAsset, ...
    // class hashes are constructed using entt type_info
    enum class AssetType : onyxU32
    {
        Invalid
    };

    enum class AssetState : onyxU8
    {
        Invalid = 0, // asset is in undefined invalid state
        Loading,
        Loaded,
        Missing, // asset is defined but can't be found
    };

    struct AssetMetaData
    {
        FilePath Path;
        AssetId Id = AssetId::Invalid;
        AssetType Type = AssetType::Invalid;
        AssetFormat Format = AssetFormat::Json;

        onyxS64 Handle = INVALID_INDEX_64;

        onyxU32 Version = 0; // Maybe not needed

        String GetName() const
        {
             return Path.stem().string();
        }

        String GetExtension() const
        {
            //TODO: remove extension once we have meta data stored on disk
            String extension = Path.extension().string();
            if (extension.empty() == false)
            {
                return extension.substr(1); // ignore .
            }

            return "";
        }
    };

    // Type trait to check if a class has Dependencies using alias
    template<typename T, typename = void>
    struct HasExtension : std::false_type {};

    template<typename T>
    struct HasExtension<T, std::void_t<typename T::Extension>> : std::true_type {};

    class AssetInterface : public RefCounted
    {
        friend struct AssetLoadRequest;
#if ONYX_IS_EDITOR
        friend struct AssetSaveRequest;
#endif
    public:
        void SetState(AssetState state) { m_State = state; }

        bool IsLoading() const { return m_State == AssetState::Loading; }
        bool IsLoaded() const { return m_State == AssetState::Loaded; }

    private:
        virtual void OnLoadFinished(AssetId id, AssetState state) = 0;
#if ONYX_IS_EDITOR
        virtual void OnSaveFinished(AssetId id, bool success) const = 0;
#endif

    protected:
        Atomic<AssetState> m_State = AssetState::Invalid;
    };

    template <typename T>
    class Asset : public AssetInterface
    {
    public:
        using AssetT = T;

        using LoadedSignalT = Signal<void(AssetHandle<AssetT>)>;
#if ONYX_IS_EDITOR
        using SavedSignalT = Signal<void(AssetHandle<AssetT>, bool)>;
#endif

    public:
        Sink<LoadedSignalT> GetOnLoadedEvent() { return Sink<LoadedSignalT>(m_LoadedSignal); }
#if ONYX_IS_EDITOR
        Sink<SavedSignalT> GetOnSavedEvent() { return Sink<SavedSignalT>(m_SavedSignal); }
#endif

    private:
        void OnLoadFinished(AssetId id, AssetState state) override
        {
            if (state == AssetState::Loaded)
            {
                Reference<AssetT> ref(this);
                m_LoadedSignal.Dispatch(AssetHandle<AssetT>( id, ref ));
            }
            
            // set to loaded after callbacks to not trigger code that depends on those callback early
            SetState(state);
        }

#if ONYX_IS_EDITOR
        void OnSaveFinished(AssetId /*id*/, bool /*success*/) const override
        {
           // Reference<AssetT> ref(this);
          //  m_SavedSignal.Dispatch({ .Id = id, .Handle = ref }, success);
        }
#endif

    private:
        LoadedSignalT m_LoadedSignal;
#if ONYX_IS_EDITOR
        SavedSignalT m_SavedSignal;
#endif
    };
}
