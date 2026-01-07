
#include <onyx/graphicscore/shader/shaderinstance.h>

#include <onyx/graphicscore/descriptorset.h>
#include <onyx/graphicscore/graphicssystem.h>
#include <onyx/graphicscore/shader/shader.h>

namespace Onyx::Graphics
{
    ShaderInstance::ShaderInstance(const GraphicsSystem& api, const PipelineHandle& pipeline, const ShaderHandle& shader)
        : m_Api(&api)
        , m_Shader(shader)
        , m_Pipeline(pipeline)
    {
        m_Shader->GetOnLoadedEvent().Connect<&ShaderInstance::OnShaderLoaded>(*this);
        if (m_Shader->IsLoaded())
        {
            OnShaderLoaded(m_Shader);
        }
    }


    DynamicArray<DescriptorSetHandle>& ShaderInstance::GetDescriptorSets(onyxU8 frameIndex)
    {
        ONYX_ASSERT(frameIndex < m_DescriptorSets.size(), "Frame index ({}) out of descriptor set bounds ({})", frameIndex, m_DescriptorSets.size());
        return m_DescriptorSets[frameIndex];
    }

    const DynamicArray<DescriptorSetHandle>& ShaderInstance::GetDescriptorSets(onyxU8 frameIndex) const
    {
        ONYX_ASSERT(frameIndex < m_DescriptorSets.size(), "Frame index ({}) out of descriptor set bounds ({})", frameIndex, m_DescriptorSets.size());
        return m_DescriptorSets[frameIndex];
    }

    bool ShaderInstance::IsCompute() const
    {
        return m_Shader && m_Shader->IsComputeShader();
    }

    bool ShaderInstance::IsValid() const
    {
        return m_Shader.IsValid() && m_Shader->IsLoaded();
    }

    void ShaderInstance::Bind(const TextureHandle& texture, const String& bindingName, onyxU8 frameIndex)
    {
        const onyxS8 descriptorSetIndex = m_BindingIdToDescriptorSet[bindingName];
        GetDescriptorSet(frameIndex, descriptorSetIndex)->Bind(texture, bindingName);
    }

    void ShaderInstance::Bind(const BufferHandle& buffer, const String& bindingName, onyxU8 frameIndex)
    {
        const onyxS8 descriptorSetIndex = m_BindingIdToDescriptorSet[bindingName];
        GetDescriptorSet(frameIndex, descriptorSetIndex)->Bind(buffer, bindingName);
    }

    void ShaderInstance::PreDraw(onyxU8 frameIndex)
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

    DescriptorSetHandle& ShaderInstance::GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex)
    {
        DynamicArray<DescriptorSetHandle>& descriptorSets = GetDescriptorSets(frameIndex);
        ONYX_ASSERT(descriptorSetIndex < descriptorSets.size());
        return descriptorSets[descriptorSetIndex];
    }

    const DescriptorSetHandle& ShaderInstance::GetDescriptorSet(onyxU8 frameIndex, onyxU8 descriptorSetIndex) const
    {
        const DynamicArray<DescriptorSetHandle>& descriptorSets = GetDescriptorSets(frameIndex);
        ONYX_ASSERT(descriptorSetIndex < descriptorSets.size());
        return descriptorSets[descriptorSetIndex];
    }

    void ShaderInstance::OnShaderLoaded(Reference<Shader>& /*shader*/)
    {
        if (m_Shader->HasDescriptorSetLayout())
        {
            StringView debugName;
            debugName = "";
            for (onyxU8 frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; ++frameIndex)
            {
                m_DescriptorSets[frameIndex] = m_Api->CreateDescriptorSet(m_Shader, debugName);
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
}
