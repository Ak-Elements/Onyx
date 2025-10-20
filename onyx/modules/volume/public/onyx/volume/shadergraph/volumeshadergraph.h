#pragma once
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Volume
{
    class VolumeShaderGraph : public Graphics::ShaderGraph
    {
        friend struct VolumeShaderGraphSerializer;

    public:
        static constexpr StringId32 TypeId{ "Onyx::Volume::Assets::VolumeShaderGraph" };
        StringId32 GetTypeId() const { return TypeId; }

        FileSystem::Filepath GetVolumeShaderGraphPath() const { return m_VolumeShaderHeaderPath; }

    private:
        FileSystem::Filepath m_VolumeShaderHeaderPath;
    };
}
