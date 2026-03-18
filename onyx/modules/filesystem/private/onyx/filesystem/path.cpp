#include <onyx/filesystem/path.h>

#include <filesystem>

#include <onyx/log/logger.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::file_system::Path
{
    namespace
    {
        constexpr StringView DEFAULT_FILE_EXTENSION = "ofile";

        HashMap<StringId32, MountPoint> MountPoints;

        Optional<MountPoint> GetMountPointFromPrefixPath(StringView path)
        {
            for (auto& [_, mountPoint] : MountPoints)
            {
                if (path.starts_with(mountPoint.Prefix))
                {
                    return mountPoint;
                }
            }
            return {};
        }

        Optional<MountPoint> GetMountPointFromPath(StringView path)
        {
            for (auto& [_, mountPoint] : MountPoints)
            {
                if (path.starts_with(mountPoint.Path.generic_string()))
                {
                    return mountPoint;
                }
            }
            return {};
        }
    }

    FilePath GetWorkingDirectory()
    {
        using namespace std::filesystem;
        return current_path();
    }

    FilePath GetTempDirectory()
    {
        return MountPoints.at(TMP_MOUNT_POINT_ID).Path;
    }

    String GetFileName(const FilePath& path)
    {
        using namespace std::filesystem;
        return path.stem().generic_string();
    }

    const HashMap<StringId32, MountPoint>& GetMountPoints()
    {
        return MountPoints;
    }

    FilePath GetFullPath(const FilePath& path)
    {
        return GetFullPath(path, DEFAULT_FILE_EXTENSION);
    }

    FilePath GetFullPath(const FilePath& path, StringView /*newExtension*/)
    {
        Optional<MountPoint> mountPoint = GetMountPointFromPrefixPath(path.string());
        if (mountPoint.has_value())
        {
            return mountPoint.value().Path / path.lexically_relative(mountPoint.value().Prefix);
        }

        return path;
    }

    FilePath GetFullPath(const String& pathStr)
    {
        return GetFullPath(pathStr, DEFAULT_FILE_EXTENSION);
    }

    FilePath GetFullPath(const String& pathStr, StringView newExtension)
    {
        return GetFullPath(FilePath(pathStr), newExtension);
    }

    FilePath GetFullPath(StringView pathStr)
    {
        return GetFullPath(pathStr, DEFAULT_FILE_EXTENSION);
    }

    FilePath GetFullPath(StringView pathStr, StringView newExtension)
    {
        return GetFullPath(FilePath(pathStr), newExtension);
    }

    FilePath GetFullPath(const char* path)
    {
        return GetFullPath(path, DEFAULT_FILE_EXTENSION);
    }

    FilePath GetFullPath(const char* path, StringView newExtension)
    {
        return GetFullPath(StringView(path), newExtension);
    }

    FilePath ReplaceExtension(const FilePath& path, StringView newExtension)
    {
        FilePath returnPath(path);
        return returnPath.replace_extension(newExtension);
    }

    FilePath ConvertToMountPath(const FilePath& absolutePath)
    {
        FilePath relativePath = absolutePath;
        if (absolutePath.is_absolute())
        {
            relativePath = absolutePath.lexically_relative(GetWorkingDirectory());
        }

        Optional<MountPoint> mountPoint = GetMountPointFromPath(relativePath.generic_string());
        if (mountPoint.has_value())
        {
            String prefixedPath = mountPoint->Prefix + relativePath.lexically_relative(mountPoint->Path).generic_string();
            ToLower(prefixedPath);
            return prefixedPath;
        }

        return relativePath;
    }

    bool Exists(const FilePath& path)
    {
        using namespace std::filesystem;
        return exists(path);
    }

    bool TempFileExists(const FilePath& path)
    {
        using namespace std::filesystem;
        return exists(GetTempDirectory().append(path.generic_string()));
    }

    void SetMountPoints(const HashMap<StringId32, MountPoint>& dataRoots)
    {
        // ensure data directories exist
        for (auto&& [identifier, mountPoint] : dataRoots)
        {
            if (identifier == TMP_MOUNT_POINT_ID)
            {
                if (Exists(mountPoint.Path) == false)
                {
                    if (CreateDirectory(mountPoint.Path) == false)
                    {
                        ONYX_LOG_ERROR("Failed to create temp directory at path {}.", mountPoint.Path);
                        continue;
                    }
                }
            }
            else if (Exists(mountPoint.Path) == false)
            {
                ONYX_LOG_ERROR("Mount point is not valid, path does not exist at path {}.", mountPoint.Path);
                continue;
            }

            MountPoints[identifier] = mountPoint;
        }
    }

    bool CreateDirectory(const FilePath& directoryPath)
    {
        if (Exists(directoryPath))
            return true;

        using namespace std::filesystem;
        std::error_code errorCode;
        create_directories(directoryPath, errorCode);

        if (errorCode.value() != 0)
        {
            ONYX_LOG_ERROR("Create directory failed, message: {}", errorCode.message().data());
            return false;
        }

        return true;
    }

    void EnumerateFiles(const FilePath& directoryPath, InplaceFunction<bool(const FilePath&)> forEach)
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (entry.is_directory())
                continue;

            if (forEach(entry.path()) == false)
                return;
        }
    }
}

namespace onyx
{
    bool Serialization<FilePath>::Serialize(Serializer& serializer, const FilePath& path)
    {
        return serializer.Write(path.generic_string());
    }

    bool Serialization<FilePath>::Deserialize(const Deserializer& deserializer, FilePath& outPath)
    {
        StringView path;
        if (deserializer.Read(path))
        {
            outPath = path;
            return true;
        }

        return false;
    }

    bool Serialization<file_system::MountPoint>::Serialize(Serializer& serializer, const file_system::MountPoint& mountPoint)
    {
        return serializer.Write<"name">(mountPoint.Prefix) &&
            serializer.Write<"path">(mountPoint.Path);
    }

    bool Serialization<file_system::MountPoint>::Deserialize(const Deserializer& deserializer, file_system::MountPoint& outMountPoint)
    {
        bool success = deserializer.Read<"name">(outMountPoint.Prefix);
        if (success == false)
            return false;

        if (outMountPoint.Prefix.ends_with(":/") == false)
        {
            outMountPoint.Prefix += ":/";
        }

        return deserializer.Read<"path">(outMountPoint.Path);
    }
}

