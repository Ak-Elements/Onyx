#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
	class Device;

	class CommandPool : public NonCopyable
	{
	public:
		CommandPool(const Device& device, onyxS32 queueFamilyIndex, VkCommandPoolCreateFlags createFlags);
		~CommandPool();

		CommandPool(CommandPool&&) = default;
		CommandPool& operator=(CommandPool&&) = default;

	private:
		const Device& m_Device;

		VULKAN_HANDLE(VkCommandPool, CommandPool, nullptr);
	};
}
