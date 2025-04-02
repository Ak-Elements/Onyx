#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
	class Device;

	class DescriptorPool : public NonCopyable
	{
	public:
		DescriptorPool(const Device& device, const DynamicArray<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateFlagBits poolCreationFlags, onyxU32 maxSets);
		~DescriptorPool();

	private:
		const Device& m_Device;
		VULKAN_HANDLE(VkDescriptorPool, DescriptorPool, nullptr);
	};
}
