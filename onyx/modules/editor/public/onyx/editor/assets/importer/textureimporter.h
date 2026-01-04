#pragma once
#include <onyx/editor/assets/importer/assetimporter.h>

namespace Onyx::Editor
{
    class TextureImporter : public AssetImporter
    {
    private:
        AssetImportResult DoImport(const FilePath& path, Assets::AssetMetaData& outAssetMeta) override;
    };
}
