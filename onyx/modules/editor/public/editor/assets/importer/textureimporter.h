#pragma once
#include <editor/assets/importer/assetimporter.h>

namespace Onyx::Editor
{
    class TextureImporter : public AssetImporter
    {
    private:
        AssetImportResult DoImport(const FileSystem::Filepath& path, Assets::AssetMetaData& outAssetMeta) override;
    };
}
