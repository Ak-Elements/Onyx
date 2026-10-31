#pragma once

#include <onyx/rhi/buffer.h>
#include <onyx/rhi/vulkan/devicememory.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class VulkanCommandBuffer;
class CommandPool;
class Device;
class VulkanGraphicsApi;

class VulkanBuffer : public Buffer, public DeviceMemory {
  public:
    VulkanBuffer( VulkanGraphicsApi& api, const BufferProperties& properties );
    ~VulkanBuffer() override;

    void Destroy();

    void* map( MapMode mode ) override;
    void unmap() override;

    void flush( uint32_t offset, uint32_t count ) override;
    void setData( uint64_t offset, const void* data, uint64_t length ) override;

    [[nodiscard]] const VkDescriptorBufferInfo& GetDescriptorInfo() const { return m_descriptorInfo; }

    [[nodiscard]] uint64_t getAliasOffset( int8_t alias ) const override {
        if( alias == InvalidIndex8 )
            return 0;
        return m_aliases[ alias ].Offset;
    }

    [[nodiscard]] uint64_t getAliasSize( int8_t alias ) const override {
        if( alias == InvalidIndex8 )
            return m_properties.m_Size;

        return m_aliases[ alias ].Size;
    }

    void clearAliases() override { m_aliases.clear(); }

    void barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess ) override;
    void barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess, int8_t aliasIndex ) override;
    int8_t alias( const BufferProperties& properties ) override;

  private:
    void init( const void* data = nullptr );
    void updateDescriptorInfo();

    [[nodiscard]] VkBufferUsageFlags getUsageFlags() const;
    static VkBufferUsageFlags getUsageFlags( const BufferProperties& properties );

    struct AliasInfo {
        uint64_t Offset = 0;
        uint64_t Size = 0;

        Access Access = Access::None;
        Context Context = Context::Graphics;

        VkBuffer Buffer = nullptr; // only used if device address is not supported
    };

    DynamicArray< AliasInfo > m_aliases;

  private:
    const Device* m_device;
    VULKAN_HANDLE( VkBuffer, buffer, nullptr );
    VkDescriptorBufferInfo m_descriptorInfo;

    Access m_access = Access::None;
    Context m_context = Context::Graphics;
};
} // namespace onyx::rhi::vulkan
