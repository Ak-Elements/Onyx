#pragma once

#include <onyx/graphics/graphicstypes.h>

namespace Onyx::Graphics
{
    class Buffer;
    struct TextureHandle;
    class Pipeline;
    class Shader;
    class ShaderCache;
    class GraphicsApi;
    class DescriptorSet;

    class ShaderEffect : public RefCounted
    {
    public:
        ShaderEffect() = default;
        ShaderEffect(const GraphicsApi& api, const Reference<Pipeline>& pipeline, const Reference<Shader>& shader);

        const Reference<Pipeline>& GetPipeline() const { return m_Pipeline; }
        DynamicArray<Reference<DescriptorSet>>& GetDescriptorSets(onyxU8 frameIndex);
        const DynamicArray<Reference<DescriptorSet>>& GetDescriptorSets(onyxU8 frameIndex) const;

        bool HasDescriptorSets() const { return m_DescriptorSets.empty() == false; }
        bool IsCompute() const;

        void Bind(const TextureHandle& texture, const String& bindingName, onyxU8 frameIndex);
        void Bind(const Reference<Buffer>& buffer, const String& bindingName, onyxU8 frameIndex);

        void PreDraw(onyxU8 frameIndex);

    private:
        Reference<DescriptorSet>& GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex);
        const Reference<DescriptorSet>& GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex) const;

    private:
        Reference<Pipeline> m_Pipeline; // node 1 Node has 1 Pipeline
        Reference<Shader> m_Shader; // node can have N shaders
                                    // node can rebind descriptor bindings
        InplaceArray<DynamicArray<Reference<DescriptorSet>>, MAX_FRAMES_IN_FLIGHT> m_DescriptorSets; // per frame
        HashMap<String, onyxU8> m_BindingIdToDescriptorSet;
    };
}
