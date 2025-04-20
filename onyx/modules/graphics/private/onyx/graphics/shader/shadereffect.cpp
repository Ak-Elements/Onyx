
#include <onyx/graphics/descriptorset.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/shader/shadereffect.h>
#include <onyx/graphics/shader/shadermodule.h>
#
namespace Onyx::Graphics
{
    ShaderEffect::ShaderEffect(const GraphicsApi& api, const PipelineHandle& pipeline, const ShaderHandle& shader)
        : m_Pipeline(pipeline)
        , m_Shader(shader)
    {
        if (shader->HasDescriptorSetLayout())
        {
            for (onyxU8 frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; ++frameIndex)
            {
                m_DescriptorSets[frameIndex] = api.CreateDescriptorSet(shader, pipeline->GetProperties().m_DebugName);
            }

            // bindings stay the same between frames so we can just peek at frame 0
            if (m_DescriptorSets.empty() == false)
            {
                const DynamicArray<DescriptorSetHandle>& descriptorSets = m_DescriptorSets[0];
                const onyxU8 descriptorSetsCount = numeric_cast<onyxU8>(descriptorSets.size());
                for (onyxU8 i = 0; i < descriptorSetsCount; ++i)
                {
                    const DescriptorSetHandle& descriptorSet = descriptorSets[i];
                    HashSet<String> bindingIds = descriptorSet->GetBindingIds();
                    for (const String& bindingId : bindingIds)
                    {
                        m_BindingIdToDescriptorSet[bindingId] = i;
                    }
                }
            }
        }
    }

    DynamicArray<DescriptorSetHandle>& ShaderEffect::GetDescriptorSets(onyxU8 frameIndex)
    {
        ONYX_ASSERT(frameIndex < m_DescriptorSets.size(), "Frame index ({}) out of descriptor set bounds ({})", frameIndex, m_DescriptorSets.size());
        return m_DescriptorSets[frameIndex];
    }

    const DynamicArray<DescriptorSetHandle>& ShaderEffect::GetDescriptorSets(onyxU8 frameIndex) const
    {
        ONYX_ASSERT(frameIndex < m_DescriptorSets.size(), "Frame index ({}) out of descriptor set bounds ({})", frameIndex, m_DescriptorSets.size());
        return m_DescriptorSets[frameIndex];
    }

    bool ShaderEffect::IsCompute() const
    {
        return m_Shader && m_Shader->IsComputeShader();
    }

    void ShaderEffect::Bind(const TextureHandle& texture, const String& bindingName, onyxU8 frameIndex)
    {
        const onyxS8 descriptorSetIndex =  m_BindingIdToDescriptorSet[bindingName];
        GetDescriptorSet(frameIndex, descriptorSetIndex)->Bind(texture, bindingName);
    }

    void ShaderEffect::Bind(const BufferHandle& buffer, const String& bindingName, onyxU8 frameIndex)
    {
        const onyxS8 descriptorSetIndex = m_BindingIdToDescriptorSet[bindingName];
        GetDescriptorSet(frameIndex, descriptorSetIndex)->Bind(buffer, bindingName);
    }

    void ShaderEffect::PreDraw(onyxU8 frameIndex)
    {
        if (m_DescriptorSets.empty())
            return;

        for (DescriptorSetHandle& descriptorSet : m_DescriptorSets[frameIndex])
        {
            if (descriptorSet->HasPendingUpdates())
            {
                descriptorSet->UpdateDescriptors();
            }
        }
    }

    DescriptorSetHandle& ShaderEffect::GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex)
    {
        DynamicArray<DescriptorSetHandle>& descriptorSets = GetDescriptorSets(frameIndex);
        ONYX_ASSERT(descriptorSetIndex < descriptorSets.size());
        return descriptorSets[descriptorSetIndex];
    }

    const DescriptorSetHandle& ShaderEffect::GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex) const
    {
        const DynamicArray<DescriptorSetHandle>& descriptorSets = GetDescriptorSets(frameIndex);
        ONYX_ASSERT(descriptorSetIndex < descriptorSets.size());
        return descriptorSets[descriptorSetIndex];
    }
}
