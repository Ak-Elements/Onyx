#pragma once

#include <onyx/rhi/texture.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Sampler;
class Device;
class VulkanTextureStorage;
class VulkanGraphicsApi;

class VulkanTexture : public rhi::Texture {
  public:
    VulkanTexture( VulkanGraphicsApi& api, const TextureProperties& properties, const VulkanTextureStorage* storage );
    VulkanTexture( VulkanGraphicsApi& api,
                   const TextureProperties& properties,
                   const VulkanTextureStorage* storage,
                   int8_t aliasIndex );
    ~VulkanTexture() override;

    [[nodiscard]]
    const VkDescriptorImageInfo& getDescriptorInfo() const {
        return m_descriptorInfo;
    }

  private:
    void init( const VulkanGraphicsApi& api, int8_t aliasIndex );
    static VkImageViewType getType( TextureType type, bool isArray );
    TextureFormat resolveTextureFormat( TextureFormat viewFormat, TextureFormat storageFormat );

    void Release() override;

  private:
    VulkanGraphicsApi* m_api = nullptr;
    VULKAN_HANDLE( VkImageView, ImageView, nullptr );

    VkDescriptorImageInfo m_descriptorInfo;
};
} // namespace onyx::rhi::vulkan
