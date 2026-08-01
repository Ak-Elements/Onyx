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
    void setData( int32_t offset, const void* data, int32_t length ) override;

    const VkDescriptorBufferInfo& GetDescriptorInfo() const { return m_DescriptorInfo; }

    uint64_t getAliasOffset( int8_t alias ) const override {
        if( alias == InvalidIndex8 )
            return 0;
        return m_Aliases[ alias ].Offset;
    }

    uint64_t getAliasSize( int8_t alias ) const override {
        if( alias == InvalidIndex8 )
            return m_properties.m_Size;

        return m_Aliases[ alias ].Size;
    }

    void clearAliases() override { m_Aliases.clear(); }

    void barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess ) override;
    void barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess, int8_t aliasIndex ) override;
    int8_t alias( const BufferProperties& properties ) override;

  private:
    void Init( const void* data = nullptr );
    void UpdateDescriptorInfo();

    VkBufferUsageFlags GetUsageFlags() const;
    static VkBufferUsageFlags GetUsageFlags( const BufferProperties& properties );

    struct AliasInfo {
        uint64_t Offset;
        uint64_t Size;

        Access Access = Access::None;
        Context Context = Context::Graphics;

        VkBuffer m_Buffer = nullptr; // only used if device address is not supported
    };

    DynamicArray< AliasInfo > m_Aliases;

  private:
    const Device* m_Device;
    VULKAN_HANDLE( VkBuffer, Buffer, nullptr );
    VkDescriptorBufferInfo m_DescriptorInfo;

    Access m_Access = Access::None;
    Context m_Context = Context::Graphics;
};
} // namespace onyx::rhi::vulkan
