#pragma once
#include <onyx/assets/asset.h>
#include <onyx/filesystem/path.h>

namespace Onyx::Editor
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

        void Import(const FileSystem::Filepath& path, Assets::AssetMetaData& outAssetMeta)
        {
            AssetImportResult result = DoImport(path, outAssetMeta);
            if (OnImportFinished)
                OnImportFinished(outAssetMeta.Id, result);
        }

        // parameters - Imported asset ID, Success/Failed flag
        Callback<void(Assets::AssetId, AssetImportResult)> OnImportFinished;

    private:
        virtual AssetImportResult DoImport(const FileSystem::Filepath& path, Assets::AssetMetaData& outAssetMeta) = 0;
    };
}
