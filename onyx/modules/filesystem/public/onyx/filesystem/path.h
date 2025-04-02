#pragma once

#include <filesystem>

template <>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    auto format(std::filesystem::path path, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", path.string());
    }
};

namespace Onyx::FileSystem
{
    using Filepath = std::filesystem::path;

    struct MountPoint
    {
        String Prefix;
        Filepath Path;
    };

    namespace Path
    {
        Filepath GetWorkingDirectory();
        Filepath GetTempDirectory();
        String GetFileName(const Filepath& path);

        const HashMap<onyxU32, MountPoint>& GetMountPoints();

        // Get an absolute path to a file in the current working directory
        // If the path is already absolute this function is a no-op
        Filepath GetFullPath(const Filepath& path);
        Filepath GetFullPath(const Filepath& path, const StringView& newExtension);
        Filepath GetFullPath(const String& path);
        Filepath GetFullPath(const String& path, const StringView& newExtension);
        Filepath GetFullPath(const StringView& path);
        Filepath GetFullPath(const StringView& path, const StringView& newExtension);
        Filepath GetFullPath(const char* path);
        Filepath GetFullPath(const char* path, const StringView& newExtension);

        Filepath ReplaceExtension(const Filepath& path, const StringView& newExtension);

        Filepath ConvertToMountPath(const Filepath& absolutePath);

        bool Exists(const Filepath& path);
        bool TempFileExists(const Filepath& path);

        void SetMountPoints(const HashMap<onyxU32, MountPoint>& mountPoints);
        
        bool CreateDirectory(const Filepath& directoryPath);
    };
}
