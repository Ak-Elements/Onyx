#pragma once

#include <onyx/graphics/texture.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class Sampler;
    class Device;
    class VulkanTextureStorage;
    class VulkanGraphicsApi;

	class VulkanTexture : public Graphics::Texture
	{
	public:
		VulkanTexture(VulkanGraphicsApi& api, const TextureProperties& properties, const VulkanTextureStorage* storage);
        VulkanTexture(VulkanGraphicsApi& api, const TextureProperties& properties, const VulkanTextureStorage* storage, onyxS8 aliasIndex);
        ~VulkanTexture() override;

        const VkDescriptorImageInfo& GetDescriptorInfo() const { return m_DescriptorInfo; }

        bool HasSampler() const { return m_Sampler.IsValid(); }

    private:
        void Init(const VulkanGraphicsApi& api, onyxS8 aliasIndex);
        void UpdateDescriptorInfo();

        static VkImageViewType GetType(TextureType type, bool isArray);
        TextureFormat ResolveTextureFormat(TextureFormat viewFormat, TextureFormat storageFormat);

        void Release() override;

    private:
        VulkanGraphicsApi* m_Api = nullptr;
        
		VULKAN_HANDLE(VkImageView, ImageView, nullptr);

        Reference<Graphics::Sampler> m_Sampler;

        VkDescriptorImageInfo m_DescriptorInfo;
	};
}
