#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Device;

class Fence : public NonCopyable {
  public:
    explicit Fence( const Device& device, bool signaled );
    Fence( Fence&& other ) noexcept;
    ~Fence();

    void Reset();
    void Wait( uint64_t timeout ) const;

  private:
    const Device& m_Device;
    VULKAN_HANDLE( VkFence, Fence, nullptr );
};
} // namespace onyx::rhi::vulkan
