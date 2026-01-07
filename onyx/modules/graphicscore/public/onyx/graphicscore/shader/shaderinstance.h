#pragma once

#include <onyx/graphicscore/graphicstypes.h>

namespace Onyx::Graphics
{
    class Buffer;
    class Pipeline;
    class Shader;
    class ShaderCache;
    class GraphicsSystem;
    class DescriptorSet;

    struct BufferHandle;
    struct TextureHandle;
    struct PipelineProperties;

    class ShaderInstance : public RefCounted
    {
    public:
        ShaderInstance() = default;
        ShaderInstance(const GraphicsSystem& api, const Reference<Pipeline>& pipeline, const Reference<Shader>& shader);

        const Reference<Pipeline>& GetPipeline() const { return m_Pipeline; }
        DynamicArray<Reference<DescriptorSet>>& GetDescriptorSets(onyxU8 frameIndex);
        const DynamicArray<Reference<DescriptorSet>>& GetDescriptorSets(onyxU8 frameIndex) const;

        bool HasDescriptorSets() const { return m_DescriptorSets.empty() == false; }
        bool IsCompute() const;

        bool IsValid() const;

        void Bind(const TextureHandle& texture, const String& bindingName, onyxU8 frameIndex);
        void Bind(const BufferHandle& buffer, const String& bindingName, onyxU8 frameIndex);

        void PreDraw(onyxU8 frameIndex);

    private:
        Reference<DescriptorSet>& GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex);
        const Reference<DescriptorSet>& GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex) const;

        void OnShaderLoaded(Reference<Shader>& shader);

    private:
        const GraphicsSystem* m_Api = nullptr;

        Reference<Shader> m_Shader;
        Reference<Pipeline> m_Pipeline;
                                    // node can rebind descriptor bindings
        InplaceArray<DynamicArray<Reference<DescriptorSet>>, MAX_FRAMES_IN_FLIGHT> m_DescriptorSets; // per frame
        HashMap<String, onyxU8> m_BindingIdToDescriptorSet;
    };
}
