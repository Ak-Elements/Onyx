#include <onyx/graphics/shader/shader.h>
#include <onyx/graphics/graphicsapi.h>

#if ONYX_USE_VULKAN
#include <onyx/graphics/vulkan/shader.h>
#endif

namespace Onyx::Graphics
{
    Reference<Shader> Shader::Create(GraphicsApi& api)
    {
        switch (api.GetApiType())
        {
        case ApiType::Vulkan:
#if ONYX_USE_VULKAN
            return Reference<Vulkan::Shader>::Create();
#else
            return nullptr;
#endif
        case ApiType::Dx12:
        case ApiType::None:
            return nullptr;
        }

        return nullptr;
    }

    DynamicArray<FileSystem::Filepath> GetShaderDirectories()
    {
        DynamicArray<FileSystem::Filepath> shaderDirectories;
        for (const FileSystem::MountPoint& mountPoint : (FileSystem::Path::GetMountPoints() | std::views::values))
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
