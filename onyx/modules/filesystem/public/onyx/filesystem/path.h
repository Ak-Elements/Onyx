#pragma once

#include <filesystem>

template <>
struct std::formatter< std::filesystem::path > : std::formatter< std::string > {
    auto format( std::filesystem::path path, format_context& ctx ) const {
        return std::format_to( ctx.out(), "{}", path.string() );
    }
};

namespace onyx::file_system {
struct MountPoint {
    String Prefix;
    FilePath Path;
};

namespace path {
static constexpr StringId32 TmpMountPointId{ "tmp:/" };

FilePath getWorkingDirectory();
FilePath getTempDirectory();
String getFileName( const FilePath& path );

const HashMap< StringId32, MountPoint >& getMountPoints();

// Get an absolute path to a file in the current working directory
// If the path is already absolute this function is a no-op
FilePath getFullPath( const FilePath& path );
FilePath getFullPath( const FilePath& path, StringView newExtension );
FilePath getFullPath( const String& path );
FilePath getFullPath( const String& path, StringView newExtension );
FilePath getFullPath( StringView path );
FilePath getFullPath( StringView path, StringView newExtension );
FilePath getFullPath( const char* path );
FilePath getFullPath( const char* path, StringView newExtension );

FilePath replaceExtension( const FilePath& path, StringView newExtension );

FilePath convertToMountPath( const FilePath& absolutePath );

bool exists( const FilePath& path );
bool tempFileExists( const FilePath& path );

void setMountPoints( const HashMap< StringId32, MountPoint >& mountPoints );

bool createDirectory( const FilePath& directoryPath );

void enumerateFiles( const FilePath& directoryPath, InplaceFunction< bool( const FilePath& ) > fileFunctor );
}; // namespace path
} // namespace onyx::file_system

namespace onyx {
template <>
struct Serialization< FilePath > {
    static bool serialize( Serializer& serializer, const FilePath& path );
    static bool deserialize( const Deserializer& deserializer, FilePath& outPath );
};

template <>
struct Serialization< file_system::MountPoint > {
    static bool serialize( Serializer& serializer, const file_system::MountPoint& mountPoint );
    static bool deserialize( const Deserializer& deserializer, file_system::MountPoint& outMountPoint );
};
} // namespace onyx
