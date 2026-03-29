#include <onyx/filesystem/path.h>

#include <filesystem>

#include <onyx/log/logger.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::file_system::path {
namespace {
constexpr StringView DefaultFileExtension = "ofile";

HashMap< StringId32, MountPoint > g_mountPoints;

Optional< MountPoint > getMountPointFromPrefixPath( StringView path ) {
    for ( auto& [ _, mountPoint ] : g_mountPoints ) {
        if ( path.starts_with( mountPoint.Prefix ) ) {
            return mountPoint;
        }
    }
    return {};
}

Optional< MountPoint > getMountPointFromPath( StringView path ) {
    for ( auto& [ _, mountPoint ] : g_mountPoints ) {
        if ( path.starts_with( mountPoint.Path.generic_string() ) ) {
            return mountPoint;
        }
    }
    return {};
}
} // namespace

FilePath getWorkingDirectory() {
    using namespace std::filesystem;
    return current_path();
}

FilePath getTempDirectory() {
    return g_mountPoints.at( TmpMountPointId ).Path;
}

String getFileName( const FilePath& path ) {
    using namespace std::filesystem;
    return path.stem().generic_string();
}

const HashMap< StringId32, MountPoint >& getMountPoints() {
    return g_mountPoints;
}

FilePath getFullPath( const FilePath& path ) {
    return getFullPath( path, DefaultFileExtension );
}

FilePath getFullPath( const FilePath& path, StringView /*newExtension*/ ) {
    Optional< MountPoint > mountPoint = getMountPointFromPrefixPath( path.string() );
    if ( mountPoint.has_value() ) {
        return mountPoint.value().Path / path.lexically_relative( mountPoint.value().Prefix );
    }

    return path;
}

FilePath getFullPath( const String& pathStr ) {
    return getFullPath( pathStr, DefaultFileExtension );
}

FilePath getFullPath( const String& pathStr, StringView newExtension ) {
    return getFullPath( FilePath( pathStr ), newExtension );
}

FilePath getFullPath( StringView pathStr ) {
    return getFullPath( pathStr, DefaultFileExtension );
}

FilePath getFullPath( StringView pathStr, StringView newExtension ) {
    return getFullPath( FilePath( pathStr ), newExtension );
}

FilePath getFullPath( const char* path ) {
    return getFullPath( path, DefaultFileExtension );
}

FilePath getFullPath( const char* path, StringView newExtension ) {
    return getFullPath( StringView( path ), newExtension );
}

FilePath replaceExtension( const FilePath& path, StringView newExtension ) {
    FilePath returnPath( path );
    return returnPath.replace_extension( newExtension );
}

FilePath convertToMountPath( const FilePath& absolutePath ) {
    FilePath relativePath = absolutePath;
    if ( absolutePath.is_absolute() ) {
        relativePath = absolutePath.lexically_relative( getWorkingDirectory() );
    }

    Optional< MountPoint > mountPoint = getMountPointFromPath( relativePath.generic_string() );
    if ( mountPoint.has_value() ) {
        String prefixedPath = mountPoint->Prefix + relativePath.lexically_relative( mountPoint->Path ).generic_string();
        toLower( prefixedPath );
        return prefixedPath;
    }

    return relativePath;
}

bool exists( const FilePath& path ) {
    return std::filesystem::exists( path );
}

bool tempFileExists( const FilePath& path ) {
    return std::filesystem::exists( getTempDirectory().append( path.generic_string() ) );
}

void setMountPoints( const HashMap< StringId32, MountPoint >& dataRoots ) {
    // ensure data directories exist
    for ( auto&& [ identifier, mountPoint ] : dataRoots ) {
        if ( identifier == TmpMountPointId ) {
            if ( path::exists( mountPoint.Path ) == false ) {
                if ( createDirectory( mountPoint.Path ) == false ) {
                    ONYX_LOG_ERROR( "Failed to create temp directory at path {}.", mountPoint.Path );
                    continue;
                }
            }
        } else if ( path::exists( mountPoint.Path ) == false ) {
            ONYX_LOG_ERROR( "Mount point is not valid, path does not exist at path {}.", mountPoint.Path );
            continue;
        }

        g_mountPoints[ identifier ] = mountPoint;
    }
}

bool createDirectory( const FilePath& directoryPath ) {
    if ( path::exists( directoryPath ) )
        return true;

    using namespace std::filesystem;
    std::error_code errorCode;
    create_directories( directoryPath, errorCode );

    if ( errorCode.value() != 0 ) {
        ONYX_LOG_ERROR( "Create directory failed, message: {}", errorCode.message().data() );
        return false;
    }

    return true;
}

void enumerateFiles( const FilePath& directoryPath, InplaceFunction< bool( const FilePath& ) > forEach ) {
    for ( const std::filesystem::directory_entry& entry :
          std::filesystem::recursive_directory_iterator( directoryPath ) ) {
        if ( entry.is_directory() )
            continue;

        if ( forEach( entry.path() ) == false )
            return;
    }
}
} // namespace onyx::file_system::path

namespace onyx {
bool Serialization< FilePath >::serialize( Serializer& serializer, const FilePath& path ) {
    return serializer.write( path.generic_string() );
}

bool Serialization< FilePath >::deserialize( const Deserializer& deserializer, FilePath& outPath ) {
    StringView path;
    if ( deserializer.read( path ) ) {
        outPath = path;
        return true;
    }

    return false;
}

bool Serialization< file_system::MountPoint >::serialize( Serializer& serializer,
                                                          const file_system::MountPoint& mountPoint ) {
    return serializer.write< "name" >( mountPoint.Prefix ) && serializer.write< "path" >( mountPoint.Path );
}

bool Serialization< file_system::MountPoint >::deserialize( const Deserializer& deserializer,
                                                            file_system::MountPoint& outMountPoint ) {
    bool success = deserializer.read< "name" >( outMountPoint.Prefix );
    if ( success == false )
        return false;

    if ( outMountPoint.Prefix.ends_with( ":/" ) == false ) {
        outMountPoint.Prefix += ":/";
    }

    return deserializer.read< "path" >( outMountPoint.Path );
}
} // namespace onyx
