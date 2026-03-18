#pragma once
#include <onyx/assets/asset.h>
#include <onyx/filesystem/path.h>

namespace onyx::editor
{
    enum AssetImportResult
    {
        Invalid,
        Success,
        FileNotFound,
        InvalidFormat,
        ParseError,
    };

    class AssetImporter
    {
    public:
        virtual ~AssetImporter() = default;

        void Import(const FilePath& path, assets::AssetMetaData& outAssetMeta)
        {
            AssetImportResult result = DoImport(path, outAssetMeta);
            if (OnImportFinished)
                OnImportFinished(outAssetMeta.Id, result);
        }

        // parameters - Imported asset ID, Success/Failed flag
        Callback<void(assets::AssetId, AssetImportResult)> OnImportFinished;

    private:
        virtual AssetImportResult DoImport(const FilePath& path, assets::AssetMetaData& outAssetMeta) = 0;
    };
}
