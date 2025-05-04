#include <onyx/graphics/vulkan/descriptorset.h>

#include <onyx/graphics/vulkan/buffer.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/vulkan/descriptorpool.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/shader.h>
#include <onyx/graphics/vulkan/texture.h>

namespace Onyx::Graphics::Vulkan
{
    DescriptorSet::DescriptorSet(const Device& device, onyxU8 set, VkDescriptorSetAllocateInfo allocateInfo)
        : Graphics::DescriptorSet(set)
        , m_Device(device)
    {
		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device.GetHandle(), &allocateInfo, &m_DescriptorSet))
    }

    DescriptorSet::DescriptorSet(const Device& device, const DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout)
        : Graphics::DescriptorSet(descriptorSetLayout.GetSet())
        , m_Device(device)
    {
        m_WriteDescriptorSets.insert(descriptorSetLayout.GetWriteDescriptors().begin(), descriptorSetLayout.GetWriteDescriptors().end());

        VkDescriptorSetAllocateInfo allocInfo{};
	    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	    allocInfo.descriptorPool = pool.GetHandle();
	    allocInfo.pSetLayouts = descriptorSetLayout.GetHandlePtr();
	    allocInfo.descriptorSetCount = 1;
        allocInfo.pNext = nullptr;

		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_Device.GetHandle(), &allocInfo, &m_DescriptorSet))
    }

    DescriptorSet::DescriptorSet(const Device& device, const DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout, const StringView& debugName)
        : DescriptorSet(device, pool, descriptorSetLayout)
    {
		SetResourceName(device.GetHandle(), VK_OBJECT_TYPE_DESCRIPTOR_SET, (onyxU64)m_DescriptorSet, debugName);
    }

    void DescriptorSet::Bind(const TextureHandle& textureHandle, const String& bindingName)
    {
	    const VulkanTexture& texture = textureHandle.Texture.As<VulkanTexture>();

		ONYX_ASSERT(m_WriteDescriptorSets.contains(bindingName), "Unknown binding name.");

		VkWriteDescriptorSet writeDescriptorSet = m_WriteDescriptorSets[bindingName];
	    writeDescriptorSet.dstSet = m_DescriptorSet;
	    writeDescriptorSet.pImageInfo = &texture.GetDescriptorInfo();
	    m_PendingDescriptorUpdates.push_back(writeDescriptorSet);
    }

    HashSet<String> DescriptorSet::GetBindingIds() const
    {
        HashSet<String> bindingIds;
        for (const String& id : (m_WriteDescriptorSets | std::ranges::views::keys))
        {
            bindingIds.emplace(id);
        }

        return bindingIds;
    }

    void DescriptorSet::Bind(const BufferHandle& bufferHandle, const String& bindingName)
    {
	    const VulkanBuffer& buffer = bufferHandle.As<VulkanBuffer>();

        if (m_WriteDescriptorSets.contains(bindingName) == false)
            return;

		ONYX_ASSERT(m_WriteDescriptorSets.contains(bindingName), "Unknown binding name.");

        VkWriteDescriptorSet writeDescriptorSet = m_WriteDescriptorSets[bindingName];
        auto it = std::ranges::find_if(m_PendingDescriptorUpdates, [&](VkWriteDescriptorSet& pendingDescriptorSet)
            {
                return pendingDescriptorSet.dstSet == m_DescriptorSet && pendingDescriptorSet.dstBinding == writeDescriptorSet.dstBinding;
            });

        if (it == m_PendingDescriptorUpdates.end())
        {
            writeDescriptorSet.dstSet = m_DescriptorSet;
            writeDescriptorSet.pBufferInfo = &buffer.GetDescriptorInfo();
            m_PendingDescriptorUpdates.push_back(writeDescriptorSet);
        }
        else
        {
            writeDescriptorSet.pBufferInfo = &buffer.GetDescriptorInfo();
        }   
    }

    void DescriptorSet::UpdateDescriptors()
    {
	    if (m_PendingDescriptorUpdates.empty())
		    return;

	    vkUpdateDescriptorSets(m_Device.GetHandle(), static_cast<onyxU32>(m_PendingDescriptorUpdates.size()), m_PendingDescriptorUpdates.data(), 0, nullptr);
	    m_PendingDescriptorUpdates.clear();
    }
}
