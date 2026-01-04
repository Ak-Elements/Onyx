#pragma once

#include <onyx/filesystem/path.h>

namespace Onyx::Assets
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
        }

        constexpr AssetId(StringView path)
            : m_Id(path.empty() ? Invalid : Hash::FNV1aHash<onyxU64>(path))
        {
        }

        explicit AssetId(const FilePath& path)
            : m_Id(path.empty() ? Invalid : Hash::FNV1aHash<onyxU64>(path.generic_string()))
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

namespace Onyx
{
    template <>
    struct Serialization<Assets::AssetId>
    {
        static bool Serialize(Serializer& serializer, const Assets::AssetId& assetId);
        static bool Deserialize(const Deserializer& deserializer, Assets::AssetId& outAssetId);
    };
}