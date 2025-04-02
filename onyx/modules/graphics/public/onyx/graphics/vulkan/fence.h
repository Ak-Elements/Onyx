#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class Device;

    class Fence : public NonCopyable
    {
    public:
        explicit Fence(const Device& device, bool signaled);
        Fence(Fence&& other) noexcept;
        ~Fence();

        void Reset();
        void Wait(onyxU64 timeout) const;

    private:
        const Device& m_Device;
        VULKAN_HANDLE(VkFence, Fence, nullptr);
    };
}
