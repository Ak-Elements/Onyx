#include <onyx/editor/assets/importer/textureimporter.h>

#include <onyx/filesystem/imagefile.h>
#include <onyx/filesystem/onyxfile.h>

namespace onyx::editor {
AssetImportResult TextureImporter::DoImport( const FilePath& path, assets::AssetMetaData& outAssetMeta ) {
    if ( file_system::Path::Exists( path ) == false )
        return AssetImportResult::FileNotFound;

    const String& extension = path.extension().string();
    if ( extension != ".png" )
        // or others
        return AssetImportResult::InvalidFormat;

    file_system::ImageFile file( path );

    FilePath importedTexturePath = file_system::Path::ReplaceExtension( path, "otex" );

    StringId32 texturePathHashed( importedTexturePath.string() );
    outAssetMeta.Id = static_cast< assets::AssetId >( texturePathHashed.getId() );
    outAssetMeta.Path = importedTexturePath;
    ++outAssetMeta.Version;

    {
        file_system::OnyxFile importedTexFile( importedTexturePath );
        file_system::FileStream binaryStream = importedTexFile.OpenStream( file_system::OpenMode::Binary |
                                                                           file_system::OpenMode::Write );

        // Write out the import format
        // Write out mips?
        binaryStream.writeRaw( file.GetChannelCount() );
        binaryStream.writeRaw( file.GetSize() );
        binaryStream.writeRaw( file.GetData() );
    }

    return AssetImportResult::Success;
}
} // namespace onyx::editor
