#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Device;

class CommandPool : public NonCopyable {
  public:
    CommandPool( const Device& device, int32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags );
    ~CommandPool() override;

    CommandPool( CommandPool&& other ) noexcept;
    CommandPool& operator=( CommandPool&& other ) noexcept;

#if !ONYX_IS_RETAIL
    void setFrameIndex( uint8_t frameIndex ) { m_frameIndex = frameIndex; }
    ONYX_NO_DISCARD uint8_t getFrameIndex() const { return m_frameIndex; }
#endif
  private:
    const Device* m_device;

    VULKAN_HANDLE( VkCommandPool, CommandPool, nullptr );

#if !ONYX_IS_RETAIL
    uint8_t m_frameIndex = 0;
#endif
};
} // namespace onyx::rhi::vulkan
