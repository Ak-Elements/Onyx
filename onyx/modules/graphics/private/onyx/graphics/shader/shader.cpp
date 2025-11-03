#include <onyx/graphics/shader/shader.h>

namespace Onyx::Graphics
{
    DynamicArray<FileSystem::Filepath> GetShaderDirectories()
    {
        DynamicArray<FileSystem::Filepath> shaderDirectories;
        for (auto& [_, mountPoint] : FileSystem::Path::GetMountPoints())
        {
            if (mountPoint.Prefix == FileSystem::Path::TMP_MOUNT_POINT_ID.GetString())
                continue;

            const FileSystem::Filepath shaderIncludePath = mountPoint.Path / "shaders/";
            if (FileSystem::Path::Exists(shaderIncludePath))
            {
                shaderDirectories.emplace_back(shaderIncludePath);
            }
        }

        return shaderDirectories;
    }
}
