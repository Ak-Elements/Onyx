#include <onyx/rhi/vulkan/descriptorpool.h>
#include <onyx/rhi/vulkan/device.h>

namespace onyx::rhi::vulkan {
DescriptorPool::DescriptorPool( const Device& device,
                                const DynamicArray< VkDescriptorPoolSize >& poolSizes,
                                VkDescriptorPoolCreateFlagBits poolCreationFlags,
                                uint32_t maxSets )
    : m_Device( device ) {
    const uint32_t poolCount = static_cast< uint32_t >( poolSizes.size() );
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = poolCreationFlags;
    poolInfo.poolSizeCount = static_cast< uint32_t >( poolCount );
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets * poolCount;

    VK_CHECK_RESULT( vkCreateDescriptorPool( m_Device.GetHandle(), &poolInfo, nullptr, &m_DescriptorPool ) );
}

DescriptorPool::~DescriptorPool() {
    if ( m_DescriptorPool != nullptr ) {
        vkDestroyDescriptorPool( m_Device.GetHandle(), m_DescriptorPool, nullptr );
        m_DescriptorPool = nullptr;
    }
}
} // namespace onyx::rhi::vulkan
