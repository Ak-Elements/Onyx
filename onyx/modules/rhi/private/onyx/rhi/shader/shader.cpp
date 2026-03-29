#include "onyx/filesystem/path.h"
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/shader.h>

#if ONYX_USE_VULKAN
#include <onyx/rhi/vulkan/shader.h>
#endif

namespace onyx::rhi {
Reference< Shader > Shader::create( const IEngine& engine ) {
    const GraphicsSystem& graphicsSystem = engine.getSystem< GraphicsSystem >();
    switch ( graphicsSystem.GetApiType() ) {
        using enum ApiType;
    case Vulkan:
#if ONYX_USE_VULKAN
        return Reference< vulkan::Shader >::create();
#else
        return nullptr;
#endif
    case Dx12:
    case None:
        return nullptr;
    }

    return nullptr;
}

DynamicArray< FilePath > GetShaderDirectories() {
    DynamicArray< FilePath > shaderDirectories;
    for ( const file_system::MountPoint& mountPoint : ( file_system::path::getMountPoints() | std::views::values ) ) {
        if ( mountPoint.Prefix == file_system::path::TmpMountPointId.getString() )
            continue;

        const FilePath shaderIncludePath = mountPoint.Path / "shaders/";
        if ( file_system::path::exists( shaderIncludePath ) ) {
            shaderDirectories.emplace_back( shaderIncludePath );
        }
    }

    return shaderDirectories;
}
} // namespace onyx::rhi
