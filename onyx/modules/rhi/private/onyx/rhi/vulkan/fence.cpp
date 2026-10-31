#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/fence.h>

namespace onyx::rhi::vulkan {
Fence::Fence( const Device& device, bool signaled )
    : m_device( device ) {
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VK_CHECK_RESULT( vkCreateFence( device.getHandle(), &fenceInfo, nullptr, &m_fence ) );
}

Fence::Fence( Fence&& other ) noexcept
    : m_device( other.m_device )
    , m_fence( other.m_fence ) {
    other.m_fence = nullptr;
}

Fence::~Fence() {
    vkDestroyFence( m_device.getHandle(), m_fence, nullptr );
    m_fence = nullptr;
}

void Fence::reset() {
    VK_CHECK_RESULT( vkResetFences( m_device.getHandle(), 1, &m_fence ) );
}

void Fence::wait( const uint64_t timeout ) const {
    VK_CHECK_RESULT( vkWaitForFences( m_device.getHandle(), 1, &m_fence, VK_TRUE, timeout ) );
}
} // namespace onyx::rhi::vulkan
