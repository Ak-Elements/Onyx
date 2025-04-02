#include <onyx/graphics/vulkan/fence.h>
#include <onyx/graphics/vulkan/device.h>

namespace Onyx::Graphics::Vulkan
{
    Fence::Fence(const Device& device, bool signaled)
        : m_Device(device)
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        VK_CHECK_RESULT(vkCreateFence(device.GetHandle(), &fenceInfo, nullptr, &m_Fence));
    }

    Fence::Fence(Fence&& other) noexcept :
        m_Device(other.m_Device),
        m_Fence(other.m_Fence)
    {
        other.m_Fence = nullptr;
    }

    Fence::~Fence()
    {
        vkDestroyFence(m_Device.GetHandle(), m_Fence, nullptr);
        m_Fence = nullptr;
    }

    void Fence::Reset()
    {
        VK_CHECK_RESULT(vkResetFences(m_Device.GetHandle(), 1, &m_Fence));
    }

    void Fence::Wait(const onyxU64 timeout) const
    {
        VK_CHECK_RESULT(vkWaitForFences(m_Device.GetHandle(), 1, &m_Fence, VK_TRUE, timeout));
    }
}
