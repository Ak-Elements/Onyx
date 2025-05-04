#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

namespace Onyx::Assets
{
    struct AssetLoadRequest;

    // AssetId is the hash of the asset full path e.g.: C:/MyProject/data/mytext.txt
    struct AssetId
    {
        static constexpr onyxU64 Invalid = 0;

        constexpr AssetId() = default;
        constexpr AssetId(onyxU64 id)
            : m_Id(id)
        {
        }

        constexpr AssetId(const char* id)
            : AssetId(FileSystem::Filepath(id))
        {
        }

        constexpr AssetId(StringView id)
            : AssetId(FileSystem::Filepath(id))
        {
        }

        explicit constexpr AssetId(const FileSystem::Filepath& path)
            : m_Id(path.empty() ? Invalid : Hash::FNV1aHash<onyxU64>(path))
        {
        }

        onyxU64 Get() const { return m_Id; }
        explicit operator onyxU64() const { return m_Id; }

        bool operator==(const AssetId& other) const { return m_Id == other.m_Id; }
        bool operator!=(const AssetId& other) const { return m_Id != other.m_Id; }

        bool IsValid() const { return m_Id != Invalid; }

    private:
        onyxU64 m_Id = Invalid;
    };


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
        String Name;
        String Extension;
        FileSystem::Filepath Path;
        AssetId Id = AssetId::Invalid;
        AssetType Type = AssetType::Invalid;

        onyxS64 Handle = INVALID_INDEX_64;

        onyxU32 Version = 0; // Maybe not needed
    };
}

template<>
struct std::hash<Onyx::Assets::AssetId>
{
    std::size_t operator()(const Onyx::Assets::AssetId& s) const noexcept
    {
        std::size_t h1 = std::hash<Onyx::onyxU64>{}(s.Get());
        return h1;
    }
};

namespace Onyx::Assets
{
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
        bool operator==(const AssetInterface& other) const { return m_Id == other.m_Id; }
        bool operator!=(const AssetInterface& other) const { return m_Id != other.m_Id; }

        AssetId GetId() const { return m_Id; }
        void SetId(const AssetId& id) { m_Id = id; }
        void SetState(AssetState state) { m_State = state; }

        bool IsLoading() const { return m_State == AssetState::Loading; }
        bool IsLoaded() const { return m_State == AssetState::Loaded; }

    private:
        virtual void OnLoadFinished() = 0;
#if ONYX_IS_EDITOR
        virtual void OnSaveFinished(bool success) const = 0;
#endif

    protected:
        AssetId m_Id = AssetId::Invalid;
        Atomic<AssetState> m_State = AssetState::Invalid;
    };

    template <typename T>
    class Asset : public AssetInterface
    {
    public:
        using AssetT = T;

        using LoadedSignalT = Signal<void(Reference<AssetT>&)>;
#if ONYX_IS_EDITOR
        using SavedSignalT = Signal<void(const Reference<AssetT>&, bool)>;
#endif

    public:
        Sink<LoadedSignalT> GetOnLoadedEvent() { return Sink<LoadedSignalT>(m_LoadedSignal); }
#if ONYX_IS_EDITOR
        Sink<SavedSignalT> GetOnSavedEvent() { return Sink<SavedSignalT>(m_SavedSignal); }
#endif

    private:
        void OnLoadFinished() override
        {
            Reference<AssetT> ref(this);
            m_LoadedSignal.Dispatch(ref);
        }

#if ONYX_IS_EDITOR
        void OnSaveFinished(bool success) const override
        {
            const Reference<AssetT> ref(this);
            m_SavedSignal.Dispatch(ref, success);
        }
#endif

    private:
        LoadedSignalT m_LoadedSignal;
#if ONYX_IS_EDITOR
        SavedSignalT m_SavedSignal;
#endif
    };
}
