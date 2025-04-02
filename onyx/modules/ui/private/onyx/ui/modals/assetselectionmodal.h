#pragma once

namespace Onyx::Assets
{
    struct AssetId;
    enum class AssetType : onyxU32;
    class AssetSystem;
}

namespace Onyx::Application::AssetSelectionControl
{
    bool Render(const Assets::AssetSystem& assetSystem, Assets::AssetType assetType, Assets::AssetId& outAssetId);
}
