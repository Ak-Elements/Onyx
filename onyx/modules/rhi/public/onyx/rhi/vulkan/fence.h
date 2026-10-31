#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Device;

class Fence : public NonCopyable {
  public:
    explicit Fence( const Device& device, bool signaled );
    Fence( Fence&& other ) noexcept;
    ~Fence() override;

    void reset();
    void wait( uint64_t timeout ) const;

  private:
    const Device& m_device;
    VULKAN_HANDLE( VkFence, fence, nullptr );
};
} // namespace onyx::rhi::vulkan
