#include <onyx/graphics/vulkan/semaphore.h>

#include <onyx/graphics/vulkan/device.h>

namespace Onyx::Graphics::Vulkan
{
    Semaphore::Semaphore(const Device& device)
        : Semaphore(device, false)
    {
    }

    Semaphore::Semaphore(const Device& device, bool isTimelineSemaphore)
        : m_Device(device)
    {
        VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        VkSemaphoreTypeCreateInfoKHR type_info = { VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR };
        type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR;
        type_info.initialValue = 0;

        if (isTimelineSemaphore)
            semaphoreInfo.pNext = &type_info;

        VK_CHECK_RESULT(vkCreateSemaphore(m_Device.GetHandle(), &semaphoreInfo, nullptr, &m_Semaphore));
    }

    Semaphore::Semaphore(Semaphore && other) noexcept
    : m_Device(other.m_Device)
    , m_Semaphore(other.m_Semaphore)
    {
	    other.m_Semaphore = nullptr;
    }

    Semaphore::~Semaphore()
    {
        vkDestroySemaphore(m_Device.GetHandle(), m_Semaphore, nullptr);
        m_Semaphore = nullptr;
    }
}
