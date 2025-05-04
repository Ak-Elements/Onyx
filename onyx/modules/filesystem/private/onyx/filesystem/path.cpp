#include <onyx/filesystem/path.h>

#include <filesystem>

#include <onyx/log/logger.h>

namespace Onyx::FileSystem::Path
{
    namespace
    {
        constexpr StringView DEFAULT_FILE_EXTENSION = "ofile";

        HashMap<StringId32, MountPoint> MountPoints;

        Optional<MountPoint> GetMountPointFromPrefixPath(const StringView& path)
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

        Optional<MountPoint> GetMountPointFromPath(const StringView& path)
        {
            for (auto& [_, mountPoint] : MountPoints)
            {
                if (path.starts_with(mountPoint.Path.string()))
                {
                    return mountPoint;
                }
            }
            return {};
        }
    }

    Filepath GetWorkingDirectory()
    {
        using namespace std::filesystem;
        return current_path();
    }

    Filepath GetTempDirectory()
    {
        return MountPoints.at(TMP_MOUNT_POINT_ID).Path;
    }

    String GetFileName(const Filepath& path)
    {
        using namespace std::filesystem;
        return path.stem().string();
    }

    const HashMap<StringId32, MountPoint>& GetMountPoints()
    {
        return MountPoints;
    }

    Filepath GetFullPath(const Filepath& path)
    {
        return GetFullPath(path, DEFAULT_FILE_EXTENSION);
    }

    Filepath GetFullPath(const Filepath& path, const StringView& /*newExtension*/)
    {
        Optional<MountPoint> mountPoint = GetMountPointFromPrefixPath(path.string());
        if (mountPoint.has_value())
        {
            return mountPoint.value().Path / path.lexically_relative(mountPoint.value().Prefix);
        }

        return path;
    }

    Filepath GetFullPath(const String& pathStr)
    {
        return GetFullPath(pathStr, DEFAULT_FILE_EXTENSION);
    }

    Filepath GetFullPath(const String& pathStr, const StringView& newExtension)
    {
        return GetFullPath(Filepath(pathStr), newExtension);
    }

    Filepath GetFullPath(const StringView& pathStr)
    {
        return GetFullPath(pathStr, DEFAULT_FILE_EXTENSION);
    }

    Filepath GetFullPath(const StringView& pathStr, const StringView& newExtension)
    {
        return GetFullPath(Filepath(pathStr), newExtension);
    }

    Filepath GetFullPath(const char* path)
    {
        return GetFullPath(path, DEFAULT_FILE_EXTENSION);
    }

    Filepath GetFullPath(const char* path, const StringView& newExtension)
    {
        return GetFullPath(StringView(path), newExtension);
    }

    Filepath ReplaceExtension(const Filepath& path, const StringView& newExtension)
    {
        Filepath returnPath(path);
        return returnPath.replace_extension(newExtension);
    }

    Filepath ConvertToMountPath(const Filepath& absolutePath)
    {
        Filepath relativePath = absolutePath;
        if (absolutePath.is_absolute())
        {
            relativePath = absolutePath.lexically_relative(GetWorkingDirectory());
        }

        Optional<MountPoint> mountPoint = GetMountPointFromPath(relativePath.string());
        if (mountPoint.has_value())
        {
            String prefixedPath = mountPoint->Prefix + relativePath.lexically_relative(mountPoint->Path).string();
            std::ranges::replace(prefixedPath, '\\', '/');
            return prefixedPath;
        }

        return {};
    }

    bool Exists(const Filepath& path)
    {
        using namespace std::filesystem;
        return exists(path);
    }

    bool TempFileExists(const Filepath& path)
    {
        using namespace std::filesystem;
        return exists(GetTempDirectory().append(path.string()));
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

    bool CreateDirectory(const Filepath& directoryPath)
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

}
