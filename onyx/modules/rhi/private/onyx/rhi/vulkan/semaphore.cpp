#include <onyx/rhi/vulkan/semaphore.h>

#include <onyx/rhi/vulkan/device.h>

namespace onyx::rhi::vulkan {
Semaphore::Semaphore( const Device& device )
    : Semaphore( device, false ) {}

Semaphore::Semaphore( const Device& device, bool isTimelineSemaphore )
    : m_device( device ) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;
    semaphoreInfo.pNext = nullptr;

    VkSemaphoreTypeCreateInfoKHR typeInfo{};
    typeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR;
    typeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR;
    typeInfo.initialValue = 0;
    typeInfo.pNext = nullptr;

    if( isTimelineSemaphore )
        semaphoreInfo.pNext = &typeInfo;

    VK_CHECK_RESULT( vkCreateSemaphore( m_device.getHandle(), &semaphoreInfo, nullptr, &m_semaphore ) );
}

Semaphore::Semaphore( Semaphore&& other ) noexcept
    : m_device( other.m_device )
    , m_semaphore( other.m_semaphore ) {
    other.m_semaphore = nullptr;
}

Semaphore::~Semaphore() {
    vkDestroySemaphore( m_device.getHandle(), m_semaphore, nullptr );
    m_semaphore = nullptr;
}
} // namespace onyx::rhi::vulkan
