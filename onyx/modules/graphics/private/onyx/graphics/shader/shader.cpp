#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/shader/shader.h>

#if ONYX_USE_VULKAN
#include <onyx/graphics/vulkan/shader.h>
#endif

namespace Onyx::Graphics
{
    Reference<Shader> Shader::Create(const IEngine& engine)
    {
        const GraphicsSystem& graphicsSystem = engine.GetSystem<GraphicsSystem>();
        switch (graphicsSystem.GetApiType())
        {
            using enum ApiType;
            case Vulkan:
    #if ONYX_USE_VULKAN
                return Reference<Vulkan::Shader>::Create();
    #else
                return nullptr;
    #endif
            case Dx12:
            case None:
                return nullptr;
        }

        return nullptr;
    }

    DynamicArray<FilePath> GetShaderDirectories()
    {
        DynamicArray<FilePath> shaderDirectories;
        for (const FileSystem::MountPoint& mountPoint : (FileSystem::Path::GetMountPoints() | std::views::values))
        {
            if (mountPoint.Prefix == FileSystem::Path::TMP_MOUNT_POINT_ID.GetString())
                continue;

            const FilePath shaderIncludePath = mountPoint.Path / "shaders/";
            if (FileSystem::Path::Exists(shaderIncludePath))
            {
                shaderDirectories.emplace_back(shaderIncludePath);
            }
        }

        return shaderDirectories;
    }
}
