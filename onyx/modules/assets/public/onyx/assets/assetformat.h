#pragma once

namespace onyx::assets
{
    enum class AssetFormat
    {
        Text,
        Binary,
        Json
   };

    template <typename T>
    concept HasAssetFormat = std::same_as<decltype(T::Format), AssetFormat>;
}
