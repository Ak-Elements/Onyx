#include <onyx/graphics/vulkan/descriptorsetlayout.h>

#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/shader.h>

#include <onyx/string/string.h>

namespace Onyx::Graphics::Vulkan
{
    DescriptorSetLayout::DescriptorSetLayout(const Device& device, onyxU8 set, const VkDescriptorSetLayoutCreateInfo& createInfo)
        : m_Device(device)
	    , m_Set(set)
    {
        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device.GetHandle(), &createInfo, nullptr, &m_Layout))
    }

    DescriptorSetLayout::DescriptorSetLayout(const Device& device, const ShaderDescriptorSet& descriptorSet)
		: m_Device(device)
	    , m_Set(descriptorSet.Set)
    {
		DynamicArray<VkDescriptorSetLayoutBinding> layoutBindings;
		for (const auto& [binding, uniformBuffer] : descriptorSet.UniformBuffers)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = ToVulkanStage(uniformBuffer.Stage);
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.binding = binding;

			String name(uniformBuffer.Id.GetString());
			ToLower(name);

			VkWriteDescriptorSet& writeDescriptorSet = m_WriteDescriptorSets[name];
			writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.dstBinding = layoutBinding.binding;
		}

		for (auto& [binding, storageBuffer] : descriptorSet.StorageBuffers)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = ToVulkanStage(storageBuffer.Stage);
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.binding = binding;
			ONYX_ASSERT(descriptorSet.UniformBuffers.find(binding) == descriptorSet.UniformBuffers.end(), "Binding is already present!");

		    String name(storageBuffer.Id.GetString());
			ToLower(name);

			VkWriteDescriptorSet& writeDescriptorSet = m_WriteDescriptorSets[name];
			writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.dstBinding = layoutBinding.binding;
		}

		for (auto& [binding, imageSampler] : descriptorSet.ImageSamplers)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layoutBinding.descriptorCount = imageSampler.ArraySize;
			layoutBinding.stageFlags = ToVulkanStage(imageSampler.Stage);
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.binding = binding;

			ONYX_ASSERT(descriptorSet.UniformBuffers.find(binding) == descriptorSet.UniformBuffers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.StorageBuffers.find(binding) == descriptorSet.StorageBuffers.end(), "Binding is already present!");

			String name = imageSampler.Name;
			ToLower(name);

			VkWriteDescriptorSet& writeDescriptorSet = m_WriteDescriptorSets[name];
			writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = imageSampler.ArraySize;
			writeDescriptorSet.dstBinding = layoutBinding.binding;
		}

		for (auto& [binding, imageSampler] : descriptorSet.SeparateTextures)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			layoutBinding.descriptorCount = imageSampler.ArraySize;
			layoutBinding.stageFlags = ToVulkanStage(imageSampler.Stage);
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.binding = binding;

			ONYX_ASSERT(descriptorSet.UniformBuffers.find(binding) == descriptorSet.UniformBuffers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.ImageSamplers.find(binding) == descriptorSet.ImageSamplers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.StorageBuffers.find(binding) == descriptorSet.StorageBuffers.end(), "Binding is already present!");

			String name = imageSampler.Name;
			ToLower(name);

			VkWriteDescriptorSet& writeDescriptorSet = m_WriteDescriptorSets[name];
			writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = imageSampler.ArraySize;
			writeDescriptorSet.dstBinding = layoutBinding.binding;
		}

		for (auto& [binding, imageSampler] : descriptorSet.SeparateSamplers)
		{
			VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			layoutBinding.descriptorCount = imageSampler.ArraySize;
			layoutBinding.stageFlags = ToVulkanStage(imageSampler.Stage);
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.binding = binding;

			ONYX_ASSERT(descriptorSet.UniformBuffers.find(binding) == descriptorSet.UniformBuffers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.ImageSamplers.find(binding) == descriptorSet.ImageSamplers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.StorageBuffers.find(binding) == descriptorSet.StorageBuffers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.SeparateTextures.find(binding) == descriptorSet.SeparateTextures.end(), "Binding is already present!");

			String name = imageSampler.Name;
			ToLower(name);

			VkWriteDescriptorSet& writeDescriptorSet = m_WriteDescriptorSets[name];
			writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = imageSampler.ArraySize;
			writeDescriptorSet.dstBinding = layoutBinding.binding;
		}

		for (auto& [bindingAndSet, imageSampler] : descriptorSet.StorageImages)
		{
            VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			layoutBinding.descriptorCount = imageSampler.ArraySize;
			layoutBinding.stageFlags = ToVulkanStage(imageSampler.Stage);
			layoutBinding.pImmutableSamplers = nullptr;

			onyxU32 binding = bindingAndSet & 0xffffffff;
			layoutBinding.binding = binding;

			ONYX_ASSERT(descriptorSet.UniformBuffers.find(binding) == descriptorSet.UniformBuffers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.StorageBuffers.find(binding) == descriptorSet.StorageBuffers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.ImageSamplers.find(binding) == descriptorSet.ImageSamplers.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.SeparateTextures.find(binding) == descriptorSet.SeparateTextures.end(), "Binding is already present!");
			ONYX_ASSERT(descriptorSet.SeparateSamplers.find(binding) == descriptorSet.SeparateSamplers.end(), "Binding is already present!");

			String name = imageSampler.Name;
			ToLower(name);

			VkWriteDescriptorSet& writeDescriptorSet = m_WriteDescriptorSets[name];
			writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.dstBinding = layoutBinding.binding;
		}

		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo{};
		descriptorLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutCreateInfo.pNext = nullptr;
		descriptorLayoutCreateInfo.bindingCount = static_cast<onyxU32>(layoutBindings.size());
		descriptorLayoutCreateInfo.pBindings = layoutBindings.data();

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_Device.GetHandle(), &descriptorLayoutCreateInfo, nullptr, &m_Layout))
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {
        if (m_Layout != nullptr)
        {
            vkDestroyDescriptorSetLayout(m_Device.GetHandle(), m_Layout, nullptr);
            m_Layout = nullptr;
        }
    }
}
