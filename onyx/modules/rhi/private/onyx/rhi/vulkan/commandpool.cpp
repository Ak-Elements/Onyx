#include <onyx/rhi/vulkan/commandpool.h>

#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
CommandPool::CommandPool( const Device& device, int32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags )
    : m_device( &device ) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags = createFlags;

    VK_CHECK_RESULT( vkCreateCommandPool( device.GetHandle(), &poolInfo, nullptr, &m_CommandPool ) )
}

CommandPool::~CommandPool() {
    if( m_CommandPool != nullptr ) {
        vkDestroyCommandPool( m_device->GetHandle(), m_CommandPool, nullptr );
        m_CommandPool = nullptr;
    }
}

CommandPool::CommandPool( CommandPool&& other ) noexcept
    : m_device( other.m_device )
    , m_CommandPool( other.m_CommandPool ) {
    other.m_device = nullptr;
    other.m_CommandPool = nullptr;
}

CommandPool& CommandPool::operator=( CommandPool&& other ) noexcept {
    std::swap( m_device, other.m_device );
    std::swap( m_CommandPool, other.m_CommandPool );
    return *this;
}
} // namespace onyx::rhi::vulkan
