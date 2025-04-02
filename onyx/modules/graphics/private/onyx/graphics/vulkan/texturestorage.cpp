#include <onyx/graphics/texture.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/vulkan/texturestorage.h>

#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/memoryallocator.h>

namespace Onyx::Graphics::Vulkan
{
    VulkanTextureStorage::VulkanTextureStorage(VulkanGraphicsApi& api, const TextureStorageProperties& properties, const Span<onyxU8>& imageData)
        : VulkanTextureStorage(api, properties)
    {
	    // upload data
	    UpdateData(api, imageData);
    }

    VulkanTextureStorage::VulkanTextureStorage(VulkanGraphicsApi& api, const TextureStorageProperties& properties)
        : TextureStorage(properties)
	    , DeviceMemory(api.GetAllocator())
	    , m_Device(&api.GetDevice())
    {
	    VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	    const onyxU32 arraySizeScale = properties.m_Type == TextureType::TextureCube ? 6 : 1;

	    createInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	    createInfo.imageType = GetType(properties.m_Type);
	    createInfo.format = GetFormat(properties.m_Format);
	    createInfo.mipLevels = properties.m_MaxMipLevel;
	    createInfo.arrayLayers = arraySizeScale * std::max<onyxU32>(properties.m_ArraySize, 1u);
	    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;// IsOptimalTiling(properties) ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
	    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	    createInfo.extent = VkExtent3D{ onyxU32(properties.m_Size[0]), onyxU32(properties.m_Size[1]), onyxU32(properties.m_Size[2]) };
	    createInfo.usage = GetUsageFlags(properties);
	    if (properties.m_Type == TextureType::TextureCube)
		    createInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	    VK_CHECK_RESULT(vkCreateImage(m_Device->GetHandle(), &createInfo, nullptr, &m_Image));

	    VkMemoryRequirements memRequirements;
	    vkGetImageMemoryRequirements(m_Device->GetHandle(), m_Image, &memRequirements);

	    VkMemoryPropertyFlags requiredMemoryPropertyFlags, preferredMemoryPropertyFlags;
	    GetMemoryPropertyFlags(m_Properties.m_CpuAccess, m_Properties.m_GpuAccess, requiredMemoryPropertyFlags, preferredMemoryPropertyFlags);

	    //TODO: memory type?
	    m_Memory = m_Allocator->AllocateDedicatedMemory(m_Image, memRequirements.memoryTypeBits, requiredMemoryPropertyFlags, preferredMemoryPropertyFlags);

	    m_IsNonCoherent = (requiredMemoryPropertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	    m_Allocator->Bind(m_Image, m_Memory);

	    SetResourceName(m_Device->GetHandle(), VK_OBJECT_TYPE_IMAGE, (onyxU64)m_Image, m_Properties.m_DebugName.empty() ? "Unnamed Storage" : m_Properties.m_DebugName.c_str());
    }

	VulkanTextureStorage::VulkanTextureStorage(VulkanGraphicsApi& api, VkImage image)
		: m_Image(image)
		, m_Device(&api.GetDevice())
	{
	}

	VulkanTextureStorage::VulkanTextureStorage(VulkanGraphicsApi& api, VkImage image, const StringView& name)
		: m_Image(image)
		, m_Device(&api.GetDevice())
	{
		SetResourceName(m_Device->GetHandle(), VK_OBJECT_TYPE_IMAGE, (onyxU64)m_Image, name);
	}

    void VulkanTextureStorage::Free(VulkanGraphicsApi& api)
    {
	    vkDestroyImage(api.GetDevice().GetHandle(), m_Image, nullptr);
	    m_Image = nullptr;
    }

    VulkanTextureStorage::~VulkanTextureStorage()
    {
		// m_Memory is only set for dedicated allocated images, swapchain images have no m_Memory 
	    if (m_Memory)
	    {
		    vkDestroyImage(m_Device->GetHandle(), m_Image, nullptr);
		    m_Image = nullptr;
	    }
    }

    VkImageAspectFlags VulkanTextureStorage::GetAspectFlags(TextureFormat format)
    {
	    if (Utils::IsDepthFormat(format))
	    {
		    if (format == TextureFormat::STENCIL_UINT8)
			    return VK_IMAGE_ASPECT_STENCIL_BIT;

		    VkFormatFeatureFlags flags = VK_IMAGE_ASPECT_DEPTH_BIT;
		    if (Utils::HasStencil(format))
			    flags |= VK_IMAGE_ASPECT_STENCIL_BIT;

		    return flags;
	    }
	    
	    return VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkImageType VulkanTextureStorage::GetType(TextureType type)
    {
	    switch (type)
	    {
            case TextureType::Texture1D: return VK_IMAGE_TYPE_1D;
            case TextureType::Texture2D: // intentional fallthrough
            case TextureType::TextureCube: return VK_IMAGE_TYPE_2D;
            case TextureType::Texture3D: return VK_IMAGE_TYPE_3D;
        }

	    ONYX_ASSERT(false, "Non supported texture type: {}", static_cast<onyxU32>(type));
	    return VK_IMAGE_TYPE_2D;
    }

    VkFormat VulkanTextureStorage::GetFormat(TextureFormat format)
    {
	    switch (format)
	    {
            case TextureFormat::Invalid: break;
            case TextureFormat::R_UNORM8: return VK_FORMAT_R8_UNORM;
            case TextureFormat::R_UINT8: return VK_FORMAT_R8_UINT;
            case TextureFormat::R_UINT16: return VK_FORMAT_R16_UINT;
            case TextureFormat::R_UINT32: return VK_FORMAT_R32_UINT;
            case TextureFormat::R_FLOAT32:  return VK_FORMAT_R32_SFLOAT;
            case TextureFormat::RG_UNORM8: return VK_FORMAT_R8G8_UNORM;
            case TextureFormat::RG_FLOAT16: return VK_FORMAT_R16G16_SFLOAT;
            case TextureFormat::RG_FLOAT32: return VK_FORMAT_R32G32_SFLOAT;
			case TextureFormat::RGB_FLOAT32: return VK_FORMAT_R32G32B32_SFLOAT;
            case TextureFormat::RGB_UNORM8: return VK_FORMAT_R8G8B8_UNORM;
            case TextureFormat::RGBA_UNORM8: return VK_FORMAT_R8G8B8A8_UNORM;
		    case TextureFormat::BGRA_UNORM8: return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::RGBA_FLOAT16: return VK_FORMAT_R16G16B16A16_SFLOAT;
            case TextureFormat::RGBA_FLOAT32: return VK_FORMAT_R32G32B32A32_SFLOAT;
            case TextureFormat::RGB_UFLOAT32_PACKED_11_11_10: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            case TextureFormat::SRGB_UNORM8: return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
            case TextureFormat::DEPTH_FLOAT32: return VK_FORMAT_D32_SFLOAT;
            case TextureFormat::DEPTH_STENCIL_UNORM24_8UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
            case TextureFormat::DEPTH_STENCIL_UNORM16_8UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
            case TextureFormat::STENCIL_UINT8: return VK_FORMAT_S8_UINT;
        }

	    ONYX_ASSERT(false, "Non supported texture format: {}", static_cast<onyxU32>(format));
	    return VK_FORMAT_UNDEFINED;
    }

    TextureFormat VulkanTextureStorage::GetFormat(VkFormat format)
    {
	    switch (format)
	    {
	        case VK_FORMAT_UNDEFINED: return TextureFormat::Invalid;
	        case VK_FORMAT_R8_UNORM: return TextureFormat::R_UNORM8;
	        case VK_FORMAT_R8_UINT: return TextureFormat::R_UINT8;
	        case VK_FORMAT_R16_UINT: return TextureFormat::R_UINT16;
	        case VK_FORMAT_R32_UINT: return TextureFormat::R_UINT32;
	        case VK_FORMAT_R32_SFLOAT: return TextureFormat::R_FLOAT32;
	        case VK_FORMAT_R8G8_UNORM: return TextureFormat::RG_UNORM8;
	        case VK_FORMAT_R16G16_SFLOAT: return TextureFormat::RG_FLOAT16;
	        case VK_FORMAT_R32G32_SFLOAT: return TextureFormat::RG_FLOAT32;
	        case VK_FORMAT_R8G8B8_UNORM: return TextureFormat::RGB_UNORM8;
	        case VK_FORMAT_R8G8B8A8_UNORM: return TextureFormat::RGBA_UNORM8;
	        case VK_FORMAT_B8G8R8A8_UNORM: return TextureFormat::BGRA_UNORM8;
	        case VK_FORMAT_R16G16B16A16_SFLOAT: return TextureFormat::RGBA_FLOAT16;
	        case VK_FORMAT_R32G32B32A32_SFLOAT: return TextureFormat::RGBA_FLOAT32;
	        case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return TextureFormat::RGB_UFLOAT32_PACKED_11_11_10;
	        case VK_FORMAT_R8G8B8A8_SRGB: return TextureFormat::SRGB_UNORM8;
	        case VK_FORMAT_D32_SFLOAT_S8_UINT: return TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT;
	        case VK_FORMAT_D32_SFLOAT: return TextureFormat::DEPTH_FLOAT32;
	        case VK_FORMAT_D24_UNORM_S8_UINT: return TextureFormat::DEPTH_STENCIL_UNORM24_8UINT;
	        case VK_FORMAT_D16_UNORM_S8_UINT: return TextureFormat::DEPTH_STENCIL_UNORM16_8UINT;
	        case VK_FORMAT_S8_UINT: return TextureFormat::STENCIL_UINT8;
	    }

	    ONYX_ASSERT(false, "Non supported texture format: {}", static_cast<onyxU32>(format));
	    return TextureFormat::Invalid;
    }

    void VulkanTextureStorage::UpdateData(VulkanGraphicsApi& api, const Span<onyxU8>& data)
    {
	    // Staging buffers for font data upload
	    BufferProperties bufferProps;
	    bufferProps.m_Size = static_cast<onyxU32>(data.size());
	    bufferProps.m_GpuAccess = Graphics::GPUAccess::Staging;
	    bufferProps.m_CpuAccess = Graphics::CPUAccess::Write;
	    VulkanBuffer stagingBuffer(api, bufferProps);

	    //stagingBuffer.Map(Graphics::MapMode::Write);
	    stagingBuffer.SetData(0, data.data(), static_cast<onyxU32>(data.size()));
	    stagingBuffer.Unmap();

	    // Copy buffer data to font image
	    api.SubmitInstantCommandBuffer(Context::Graphics, 0, [&](CommandBuffer& commandBuffer)
	    {
			VulkanCommandBuffer& vulkanCmdBuffer = static_cast<VulkanCommandBuffer&>(commandBuffer);
			// Copy
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.bufferOffset = 0;
			bufferCopyRegion.bufferRowLength = 0;
			bufferCopyRegion.bufferImageHeight = 0;

            bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferCopyRegion.imageSubresource.mipLevel = 0;
            bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
            bufferCopyRegion.imageSubresource.layerCount = 1;

			bufferCopyRegion.imageOffset = { 0, 0, 0 };
			bufferCopyRegion.imageExtent = { static_cast<onyxU32>(m_Properties.m_Size[0]), static_cast<onyxU32>(m_Properties.m_Size[1]), static_cast<onyxU32>(m_Properties.m_Size[2]) };

			TransitionLayout(vulkanCmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_2_TRANSFER_WRITE_BIT, 0, 1);

		    //commandBuffer.
			vkCmdCopyBufferToImage(vulkanCmdBuffer.GetHandle(), stagingBuffer.GetHandle(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

		    // prepare first mip
			if (m_Properties.m_MaxMipLevel > 1)
				TransitionLayout(vulkanCmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_2_TRANSFER_READ_BIT, 0, 1);

			onyxS32 width = m_Properties.m_Size[0];
			onyxS32 height = m_Properties.m_Size[1];

			for (onyxU32 mipIndex = 1; mipIndex < m_Properties.m_MaxMipLevel; ++mipIndex)
			{
				TransitionLayout(vulkanCmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_2_TRANSFER_WRITE_BIT, 0, 1);

				VkImageBlit blitRegion{};
				blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blitRegion.srcSubresource.mipLevel = mipIndex - 1;
				blitRegion.srcSubresource.baseArrayLayer = 0;
				blitRegion.srcSubresource.layerCount = 1;

				blitRegion.srcOffsets[0] = { 0, 0, 0 };
				blitRegion.srcOffsets[1] = { width, height, 1 };

				width /= 2;
				height /= 2;

				blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blitRegion.dstSubresource.mipLevel = mipIndex;
				blitRegion.dstSubresource.baseArrayLayer = 0;
				blitRegion.dstSubresource.layerCount = 1;

				blitRegion.dstOffsets[0] = { 0, 0, 0 };
				blitRegion.dstOffsets[1] = { width, height, 1 };

				vkCmdBlitImage(vulkanCmdBuffer.GetHandle(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_LINEAR);

			}

			TransitionLayout(vulkanCmdBuffer, Context::Graphics, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT, 0, 1);
	    });

		stagingBuffer.Destroy();
    }

    onyxS8 VulkanTextureStorage::Alias(const TextureStorageProperties& aliasProperties)
    {
	    ONYX_ASSERT(m_Image != nullptr, "Image is already allocated.");

	    VkImageCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	    const onyxU32 arraySizeScale = aliasProperties.m_Type == TextureType::TextureCube ? 6 : 1;

	    createInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	    createInfo.imageType = GetType(aliasProperties.m_Type);
	    createInfo.format = GetFormat(aliasProperties.m_Format);
	    createInfo.mipLevels = aliasProperties.m_MaxMipLevel;
	    createInfo.arrayLayers = arraySizeScale * std::max<onyxU32>(aliasProperties.m_ArraySize, 1u);
	    createInfo.samples = VK_SAMPLE_COUNT_8_BIT;
	    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;// IsOptimalTiling(properties) ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
	    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	    createInfo.extent = VkExtent3D{ static_cast<onyxU32>(aliasProperties.m_Size[0]), static_cast<onyxU32>(aliasProperties.m_Size[1]), static_cast<onyxU32>(aliasProperties.m_Size[2]) };
	    createInfo.usage = GetUsageFlags(aliasProperties);
	    if (aliasProperties.m_Type == TextureType::TextureCube)
		    createInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	    // move to device memory
	    onyxS8 aliasIndex = static_cast<onyxS8>(m_Aliases.size());
        m_Aliases.Add(m_Allocator->Alias(m_Memory, createInfo));

	    return aliasIndex;
    }

    void VulkanTextureStorage::TransitionLayout(VulkanCommandBuffer& commandBuffer, Context newContext, VkImageLayout newLayout, VkAccessFlags2 newAccess, onyxU32 mipLevel, onyxU32 mipCount)
    {
		VkImageMemoryBarrier2KHR barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR };
		barrier.srcAccessMask = static_cast<VkAccessFlags2>(m_Access);
		barrier.srcStageMask = GetPipelineFlags(barrier.srcAccessMask, newContext);
		barrier.dstAccessMask = newAccess;
		barrier.dstStageMask = GetPipelineFlags(barrier.dstAccessMask, newContext);
		barrier.oldLayout = static_cast<VkImageLayout>(m_Layout);
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_Image;
		barrier.subresourceRange.aspectMask = Utils::IsDepthFormat(m_Properties.m_Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseMipLevel = mipLevel;
		barrier.subresourceRange.levelCount = mipCount;

		VkDependencyInfoKHR dependency_info{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR };
		dependency_info.imageMemoryBarrierCount = 1;
		dependency_info.pImageMemoryBarriers = &barrier;

		vkCmdPipelineBarrier2(commandBuffer.GetHandle(), &dependency_info);

		m_Layout = static_cast<onyxU32>(newLayout);
		m_Access = static_cast<onyxU32>(newAccess);

        /*
	    VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	    barrier.image = m_Image;
	    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	    barrier.subresourceRange.aspectMask = Utils::IsDepthFormat(m_Properties.m_Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	    barrier.subresourceRange.baseArrayLayer = 0;
	    barrier.subresourceRange.layerCount = 1;
	    barrier.subresourceRange.levelCount = mipCount;

	    barrier.subresourceRange.baseMipLevel = mipLevel;
	    barrier.oldLayout = static_cast<VkImageLayout>(m_Layout);
	    barrier.newLayout = newLayout;
	    barrier.srcAccessMask = static_cast<VkAccessFlags>(m_Access);
	    barrier.dstAccessMask = newAccess;

	    const VkPipelineStageFlags sourceStage = GetPipelineFlags(barrier.srcAccessMask, Context::Graphics);
	    const VkPipelineStageFlags destinationStage = GetPipelineFlags(barrier.dstAccessMask, Context::Graphics);

	    vkCmdPipelineBarrier(commandBuffer.GetHandle(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	    m_Layout = static_cast<onyxU32>(newLayout);
	    m_Access = static_cast<onyxU32>(newAccess);*/
    }

    VkImageUsageFlags VulkanTextureStorage::GetUsageFlags(const TextureStorageProperties& properties)
    {
	    VkImageUsageFlags usageFlags = 0;

	    switch (properties.m_GpuAccess)
	    {
	    case GPUAccess::Read:
	    case GPUAccess::Write:
		    usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		    break;

	    case GPUAccess::Staging:
		    if (properties.m_CpuAccess == CPUAccess::Write) // upload staging texture
			    usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		    else
			    usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		    break;
	    default:
		    ONYX_ASSERT(false, "Illegal GPU access value: 0x{:x}", Enums::ToIntegral(properties.m_GpuAccess));
	    }

	    if (properties.m_IsTexture)
		    usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

	    if (properties.m_IsFrameBuffer)
	    {
		    if (Utils::IsDepthFormat(properties.m_Format))
			    usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		    else
			    usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	    }

	    if (properties.m_IsWritable)
		    usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;

	    return usageFlags;
    }

    bool VulkanTextureStorage::IsOptimalTiling(const TextureStorageProperties& properties)
    {
	    return properties.m_CpuAccess == CPUAccess::None || properties.m_CpuAccess == CPUAccess::UpdateKeep;
    }
}
