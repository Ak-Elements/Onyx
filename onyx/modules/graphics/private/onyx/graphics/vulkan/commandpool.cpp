#include <onyx/graphics/vulkan/commandpool.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
	CommandPool::CommandPool(const Device& device, onyxS32 queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
        : m_Device(&device)
    {
	    VkCommandPoolCreateInfo poolInfo = {};
	    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	    poolInfo.queueFamilyIndex = queueFamilyIndex;
	    poolInfo.flags = createFlags;

	    VK_CHECK_RESULT(vkCreateCommandPool(device.GetHandle(), &poolInfo, nullptr, &m_CommandPool))
    }

    CommandPool::~CommandPool()
    {
	    if (m_CommandPool != nullptr)
	    {
		    vkDestroyCommandPool(m_Device->GetHandle(), m_CommandPool, nullptr);
		    m_CommandPool = nullptr;
	    }
    }

    CommandPool::CommandPool(CommandPool&& other)
        : m_Device(other.m_Device)
        , m_CommandPool(other.m_CommandPool)
    {
        other.m_Device = nullptr;
        other.m_CommandPool = nullptr;
    }

    CommandPool& CommandPool::operator=(CommandPool&& other)
    {
        std::swap(m_Device, other.m_Device);
        std::swap(m_CommandPool, other.m_CommandPool);
        return *this;
    }
}
