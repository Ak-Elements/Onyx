#pragma once

#include <filesystem>

template <>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
    auto format(std::filesystem::path path, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", path.string());
    }
};

namespace onyx::file_system
{
    struct MountPoint
    {
        String Prefix;
        FilePath Path;
    };

    namespace Path
    {
        static constexpr StringId32 TMP_MOUNT_POINT_ID { "tmp:/" };

        FilePath GetWorkingDirectory();
        FilePath GetTempDirectory();
        String GetFileName(const FilePath& path);

        const HashMap<StringId32, MountPoint>& GetMountPoints();

        // Get an absolute path to a file in the current working directory
        // If the path is already absolute this function is a no-op
        FilePath GetFullPath(const FilePath& path);
        FilePath GetFullPath(const FilePath& path, StringView newExtension);
        FilePath GetFullPath(const String& path);
        FilePath GetFullPath(const String& path, StringView newExtension);
        FilePath GetFullPath(StringView path);
        FilePath GetFullPath(StringView path, StringView newExtension);
        FilePath GetFullPath(const char* path);
        FilePath GetFullPath(const char* path, StringView newExtension);

        FilePath ReplaceExtension(const FilePath& path, StringView newExtension);

        FilePath ConvertToMountPath(const FilePath& absolutePath);

        bool Exists(const FilePath& path);
        bool TempFileExists(const FilePath& path);

        void SetMountPoints(const HashMap<StringId32, MountPoint>& mountPoints);
        
        bool CreateDirectory(const FilePath& directoryPath);

        void EnumerateFiles(const FilePath& directoryPath, InplaceFunction<bool(const FilePath&)> fileFunctor);
    };
}

namespace onyx
{
    template <>
    struct Serialization<FilePath>
    {
        static bool serialize(Serializer& serializer, const FilePath& path);
        static bool deserialize(const Deserializer& deserializer, FilePath& outPath);
    };

    template <>
    struct Serialization<file_system::MountPoint>
    {
        static bool serialize(Serializer& serializer, const file_system::MountPoint& mountPoint);
        static bool deserialize(const Deserializer& deserializer, file_system::MountPoint& outMountPoint);
    };
}
