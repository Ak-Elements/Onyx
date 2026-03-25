#pragma once

#include <onyx/rhi/texturestorage.h>
#include <onyx/rhi/vulkan/devicememory.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class PhysicalDevice;
class CommandPool;
class Device;
class VulkanCommandBuffer;

class VulkanTextureStorage : public TextureStorage, public DeviceMemory {
  public:
    VulkanTextureStorage( VulkanGraphicsApi& api, const TextureStorageProperties& properties );
    VulkanTextureStorage( VulkanGraphicsApi& api,
                          const TextureStorageProperties& properties,
                          const Span< uint8_t >& imageData );

    // swapchain images
    VulkanTextureStorage( VulkanGraphicsApi& api, VkImage image );
    VulkanTextureStorage( VulkanGraphicsApi& api, VkImage image, StringView name );

    ~VulkanTextureStorage() override;
    void Free( VulkanGraphicsApi& api );

    VkFormatFeatureFlags GetFormatFlags() const { return GetFormatFlags( m_Properties.m_Format, false ); }
    static VkFormatFeatureFlags GetFormatFlags( TextureFormat format, bool isDepthStencil );

    static VkImageAspectFlags GetAspectFlags( TextureFormat format );
    static VkFormat GetFormat( TextureFormat format );
    static TextureFormat GetFormat( VkFormat format );

    void UpdateData( VulkanGraphicsApi& api, const Span< uint8_t >& data );

    int8_t Alias( const TextureStorageProperties& aliasProperties );

    bool HasAlias( int8_t aliasIndex ) { return aliasIndex < m_Aliases.size(); }
    VkImage GetAliasHandle( int8_t aliasIndex ) const {
        ONYX_ASSERT( aliasIndex < m_Aliases.size() );
        return m_Aliases[ aliasIndex ];
    }

    void TransitionPresent( VulkanCommandBuffer& commandBuffer );

    void TransitionLayout( CommandBuffer& commandBuffer,
                           Context newContext,
                           Access newAccess,
                           ImageLayout newLayout ) override;

  private:
    static VkImageType GetType( TextureType type );
    static VkImageUsageFlags GetUsageFlags( const TextureStorageProperties& properties );
    static bool IsOptimalTiling( const TextureStorageProperties& properties );

    const Device* m_Device;

    VULKAN_HANDLE( VkImage, Image, nullptr );

    InplaceArray< VkImage, 8 > m_Aliases; // max 8 aliases?

    ImageLayout m_Layout = ImageLayout::None;
    Access m_Access = Access::None;
};
} // namespace onyx::rhi::vulkan
