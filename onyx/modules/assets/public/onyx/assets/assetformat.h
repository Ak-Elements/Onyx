#pragma once

namespace Onyx::Assets
{
    enum class AssetFormat
    {
        Text,
        Binary,
        Json
   };

    template <typename T>
    concept HasAssetFormat = requires
    {
        std::same_as<decltype(T::Format), AssetFormat>;
    };
}
