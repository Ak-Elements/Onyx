#pragma once

#include <onyx/filesystem/path.h>

namespace onyx::assets
{
    // AssetId is the hash of the asset full path e.g.: C:/MyProject/data/mytext.txt
    struct AssetId
    {
        static constexpr onyxU64 Invalid = 0;

        constexpr AssetId() = default;
        constexpr AssetId(onyxU64 id)
            : m_Id(id)
        {
        }

        constexpr AssetId(const char* path)
            : AssetId(StringView(path))
        {
#if ONYX_IS_DEBUG
            m_Path = path;
#endif
        }

        constexpr AssetId(StringView path)
            : m_Id(path.empty() ? Invalid : hash::FNV1aHash<onyxU64>(path))
#if ONYX_IS_DEBUG
            , m_Path(path)
#endif
        {
            
        }

        explicit AssetId(const FilePath& path)
            : m_Id(path.empty() ? Invalid : hash::FNV1aHash<onyxU64>(path.generic_string()))
#if ONYX_IS_DEBUG
            , m_Path(path.generic_string())
#endif
        {
        }

        onyxU64 Get() const { return m_Id; }
        explicit operator onyxU64() const { return m_Id; }

        bool operator==(const AssetId& other) const { return m_Id == other.m_Id; }
        bool operator!=(const AssetId& other) const { return m_Id != other.m_Id; }

        bool IsValid() const { return m_Id != Invalid; }

#if ONYX_IS_DEBUG
        StringView GetPath() const { return StringView(m_Path); }
#endif

    private:
        onyxU64 m_Id = Invalid;
#if ONYX_IS_DEBUG
        String m_Path;
#endif
    };
}

namespace onyx
{
    template <>
    struct Serialization<assets::AssetId>
    {
        static bool Serialize(Serializer& serializer, const assets::AssetId& assetId);
        static bool Deserialize(const Deserializer& deserializer, assets::AssetId& outAssetId);
    };
}

template<>
struct std::hash<onyx::assets::AssetId>
{
    std::size_t operator()(const onyx::assets::AssetId& s) const noexcept
    {
        std::size_t h1 = std::hash<onyx::onyxU64>{}(s.Get());
        return h1;
    }
};

template <>
struct std::formatter<onyx::assets::AssetId> : std::formatter<std::string>
{
    auto format(onyx::assets::AssetId id, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{:x}", id.Get());
    }
};