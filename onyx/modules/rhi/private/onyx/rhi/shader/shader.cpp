#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/shader.h>

#if ONYX_USE_VULKAN
#include <onyx/rhi/vulkan/shader.h>
#endif

namespace onyx::rhi
{
    Reference<Shader> Shader::Create(const IEngine& engine)
    {
        const GraphicsSystem& graphicsSystem = engine.GetSystem<GraphicsSystem>();
        switch (graphicsSystem.GetApiType())
        {
            using enum ApiType;
            case Vulkan:
    #if ONYX_USE_VULKAN
                return Reference<vulkan::Shader>::Create();
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
        for (const file_system::MountPoint& mountPoint : (file_system::Path::GetMountPoints() | std::views::values))
        {
            if (mountPoint.Prefix == file_system::Path::TMP_MOUNT_POINT_ID.GetString())
                continue;

            const FilePath shaderIncludePath = mountPoint.Path / "shaders/";
            if (file_system::Path::Exists(shaderIncludePath))
            {
                shaderDirectories.emplace_back(shaderIncludePath);
            }
        }

        return shaderDirectories;
    }
}
