#pragma once
#include <onyx/editor/assets/importer/assetimporter.h>

namespace onyx::editor
{
    class TextureImporter : public AssetImporter
    {
    private:
        AssetImportResult DoImport(const FilePath& path, assets::AssetMetaData& outAssetMeta) override;
    };
}
