#include <editor/assets/importer/textureimporter.h>

#include <onyx/filesystem/imagefile.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Editor
{
    AssetImportResult TextureImporter::DoImport(const FileSystem::Filepath& path, Assets::AssetMetaData& outAssetMeta)
    {
        if (FileSystem::Path::Exists(path) == false)
            return AssetImportResult::FileNotFound;

        const String& extension = path.extension().string();
        if (extension != ".png")
            // or others
            return AssetImportResult::InvalidFormat;

        FileSystem::ImageFile file(path);
        
        FileSystem::Filepath importedTexturePath = FileSystem::Path::ReplaceExtension(path, "otex");

        outAssetMeta.Id = static_cast<Assets::AssetId>(Hash::FNV1aHash64(importedTexturePath.string()));
        outAssetMeta.Path = importedTexturePath;
        ++outAssetMeta.Version;

        {
            FileSystem::OnyxFile importedTexFile(importedTexturePath);
            FileSystem::FileStream binaryStream = importedTexFile.OpenStream(FileSystem::OpenMode::Binary | FileSystem::OpenMode::Write);

            // Write out the import format
            // Write out mips?
            binaryStream.WriteRaw(file.GetChannelCount());
            binaryStream.WriteRaw(file.GetSize());
            binaryStream.WriteRaw(file.GetData());
        }

        return AssetImportResult::Success;
    }
}
