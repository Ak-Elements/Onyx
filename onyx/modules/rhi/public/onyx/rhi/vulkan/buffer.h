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

    void* Map( MapMode mode ) override;
    void Unmap() override;

    void Flush( uint32_t offset, uint32_t count ) override;
    void SetData( int32_t offset, const void* data, int32_t length ) override;

    const VkDescriptorBufferInfo& GetDescriptorInfo() const { return m_DescriptorInfo; }

    uint64_t GetAliasOffset( int8_t alias ) const override {
        if ( alias == InvalidIndex8 )
            return 0;
        return m_Aliases[ alias ].Offset;
    }

    uint64_t GetAliasSize( int8_t alias ) const override {
        if ( alias == InvalidIndex8 )
            return m_Properties.m_Size;

        return m_Aliases[ alias ].Size;
    }

    void ClearAliases() override { m_Aliases.clear(); }

    void Barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess ) override;
    void Barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess, int8_t aliasIndex ) override;
    int8_t Alias( const BufferProperties& properties ) override;

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
