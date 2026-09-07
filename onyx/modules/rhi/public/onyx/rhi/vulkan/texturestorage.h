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
    void free( VulkanGraphicsApi& api );

    [[nodiscard]] VkFormatFeatureFlags getFormatFlags() const { return getFormatFlags( m_properties.Format, false ); }
    static VkFormatFeatureFlags getFormatFlags( TextureFormat format, bool isDepthStencil );

    static VkImageAspectFlags getAspectFlags( TextureFormat format );
    static VkFormat getFormat( TextureFormat format );
    static TextureFormat getFormat( VkFormat format );

    void updateData( VulkanGraphicsApi& api, const Span< uint8_t >& data );

    int8_t alias( const TextureStorageProperties& aliasProperties );

    bool hasAlias( int8_t aliasIndex ) { return aliasIndex < m_aliases.size(); }
    [[nodiscard]] VkImage getAliasHandle( int8_t aliasIndex ) const {
        ONYX_ASSERT( aliasIndex < m_aliases.size() );
        return m_aliases[ aliasIndex ];
    }

    void transitionPresent( VulkanCommandBuffer& commandBuffer );

    void transitionLayout( CommandBuffer& commandBuffer,
                           Context newContext,
                           Access newAccess,
                           ImageLayout newLayout ) override;

  private:
    static VkImageType getType( TextureType type );
    static VkImageUsageFlags getUsageFlags( const TextureStorageProperties& properties );
    static bool isOptimalTiling( const TextureStorageProperties& properties );

    const Device* m_device;

    VULKAN_HANDLE( VkImage, image, nullptr );

    InplaceArray< VkImage, 8 > m_aliases; // max 8 aliases?

    ImageLayout m_layout = ImageLayout::None;
    Access m_access = Access::None;
};
} // namespace onyx::rhi::vulkan
