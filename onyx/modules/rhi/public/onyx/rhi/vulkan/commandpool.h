#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan
{
	class Device;

	class CommandPool : public NonCopyable
	{
	public:
		CommandPool(const Device& device, onyxS32 queueFamilyIndex, VkCommandPoolCreateFlags createFlags);
		~CommandPool();

		CommandPool(CommandPool&& other);
		CommandPool& operator=(CommandPool&& other);

	private:
		const Device* m_Device;

		VULKAN_HANDLE(VkCommandPool, CommandPool, nullptr);
	};
}
