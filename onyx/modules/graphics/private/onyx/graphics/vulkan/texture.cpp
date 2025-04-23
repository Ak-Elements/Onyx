#include <onyx/graphics/vulkan/texture.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/sampler.h>
#include <onyx/graphics/vulkan/texturestorage.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{

VulkanTexture::VulkanTexture(VulkanGraphicsApi& api, const TextureProperties& properties, const VulkanTextureStorage* storage)
    : VulkanTexture(api, properties, storage, INVALID_INDEX_8)
{
}

VulkanTexture::VulkanTexture(VulkanGraphicsApi& api, const TextureProperties& properties, const VulkanTextureStorage* storage, onyxS8 aliasIndex)
	: Texture(properties, storage)
	, m_Api(&api)
{
	Init(api, aliasIndex);
}

void VulkanTexture::Init(const VulkanGraphicsApi& api, onyxS8 aliasIndex)
{
	const Device& device = api.GetDevice();
	const VulkanTextureStorage* textureStorage = static_cast<const VulkanTextureStorage*>(m_Storage);

	const VkImageAspectFlags aspectMask = VulkanTextureStorage::GetAspectFlags(m_Properties.m_Format);

	const TextureStorageProperties& storageProperties = m_Storage->GetProperties();
	const onyxU32 storageArraySize = std::max<onyxU16>(storageProperties.m_ArraySize, 1u);
	const onyxU32 arraySize = m_Properties.m_ArraySize ? m_Properties.m_ArraySize : storageArraySize - m_Properties.m_ArrayIndex;
	ONYX_ASSERT(arraySize <= storageArraySize);

	VkImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = (aliasIndex == INVALID_INDEX_8) ? textureStorage->GetHandle() : textureStorage->GetAliasHandle(aliasIndex);
	imageViewCreateInfo.pNext = nullptr;

	const onyxU32 mip = m_Properties.m_MipLevel;
	const onyxU32 miplevels = m_Properties.m_MaxMipLevel ? m_Properties.m_MaxMipLevel : storageProperties.m_MaxMipLevel - mip;
	ONYX_ASSERT(mip + miplevels <= storageProperties.m_MaxMipLevel);

	imageViewCreateInfo.format = VulkanTextureStorage::GetFormat(m_Properties.m_Format);
	imageViewCreateInfo.components = VkComponentMapping{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	imageViewCreateInfo.subresourceRange = VkImageSubresourceRange{ aspectMask, mip, miplevels, m_Properties.m_ArrayIndex, arraySize };

	if (storageProperties.m_Type == TextureType::TextureCube)
	{
		imageViewCreateInfo.subresourceRange.layerCount *= 6;
		imageViewCreateInfo.viewType = m_Properties.m_IsWriteable || m_Properties.m_AllowCubeMapLoads ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
	}
	else
		imageViewCreateInfo.viewType = GetType(storageProperties.m_Type, storageProperties.m_ArraySize != 0);

	VK_CHECK_RESULT(vkCreateImageView(device.GetHandle(), &imageViewCreateInfo, nullptr, &m_ImageView))
	SetResourceName(device.GetHandle(), VK_OBJECT_TYPE_IMAGE_VIEW, (onyxU64)m_ImageView, m_Properties.m_DebugName.empty() ? "Unnamed Texture" : m_Properties.m_DebugName.c_str());

	m_Sampler = api.GetSampler(m_Properties.m_Sampler);
	UpdateDescriptorInfo();
}

VulkanTexture::~VulkanTexture()
{
	if (m_ImageView != nullptr)
	{
		vkDestroyImageView(m_Api->GetDevice().GetHandle(), m_ImageView, nullptr);
        m_ImageView = nullptr;
	}
}

void VulkanTexture::Release()
{
	m_Api->ReleaseTexture(*this);
}

void VulkanTexture::UpdateDescriptorInfo()
{
	if (Utils::IsDepthFormat(m_Properties.m_Format))
		m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	else if (m_Properties.m_Usage == TextureUsage::Storage)
		m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	else
		m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (m_Properties.m_Usage == TextureUsage::Storage)
		m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	else if (m_Properties.m_Usage == TextureUsage::HostRead)
		m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	m_DescriptorInfo.imageView = m_ImageView;
	m_DescriptorInfo.sampler = m_Sampler ? m_Sampler.As<Sampler>().GetHandle() : VK_NULL_HANDLE;
}

VkImageViewType VulkanTexture::GetType(TextureType type, bool isArray)
{
	switch (type)
	{
		case TextureType::Texture1D:
		return isArray ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
	case TextureType::Texture2D:
		return isArray ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
	case TextureType::Texture3D:
		ONYX_ASSERT(!isArray, "3D texture does not support array texture view");
		return VK_IMAGE_VIEW_TYPE_3D;
	case TextureType::TextureCube:
		return isArray ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
	default:
		ONYX_ASSERT(false, "Unknown storage type: {}, {}", Enums::ToString(type), isArray);
		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}

}
