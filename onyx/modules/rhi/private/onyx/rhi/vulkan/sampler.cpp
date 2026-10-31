#include <onyx/rhi/vulkan/sampler.h>

#include <onyx/rhi/vulkan/device.h>

namespace onyx::rhi::vulkan {
namespace {
VkSamplerAddressMode toVulkanAddressMode( SamplerAddressMode mode ) {
    switch( mode ) {
    case SamplerAddressMode::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case SamplerAddressMode::MirroredRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case SamplerAddressMode::ClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case SamplerAddressMode::ClampToBorder:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case SamplerAddressMode::MirrorClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    default:
        ONYX_ASSERT( false, "Unhandeled sampler address mode %s", enums::toString( mode ) );
        return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
    }
}

VkFilter toVulkanSamplerFilter( SamplerFilter mode ) {
    switch( mode ) {
    case SamplerFilter::Nearest:
        return VK_FILTER_NEAREST;
    case SamplerFilter::Linear:
        return VK_FILTER_LINEAR;
    case SamplerFilter::Cubic:
        return VK_FILTER_CUBIC_EXT;
    default:
        ONYX_ASSERT( false, "Unhandeled sampler filter mode %s", enums::toString( mode ) );
        return VK_FILTER_MAX_ENUM;
    }
}

VkSamplerMipmapMode toVulkanSamplerMipMapMode( SamplerMipMapMode mode ) {
    switch( mode ) {
    case SamplerMipMapMode::Nearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case SamplerMipMapMode::Linear:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    default:
        ONYX_ASSERT( false, "Unhandeled sampler mipmap mode %s", enums::toString( mode ) );
        return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
    }
}
} // namespace

Sampler::Sampler( const Device& device, const SamplerProperties& properties )
    : m_device( device ) {
    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.addressModeU = toVulkanAddressMode( properties.AddressModeU );
    createInfo.addressModeV = toVulkanAddressMode( properties.AddressModeV );
    createInfo.addressModeW = toVulkanAddressMode( properties.AddressModeW );
    createInfo.minFilter = toVulkanSamplerFilter( properties.MinFilter );
    createInfo.magFilter = toVulkanSamplerFilter( properties.MagFilter );
    createInfo.mipmapMode = toVulkanSamplerMipMapMode( properties.MipFilter );
    createInfo.anisotropyEnable = true;
    createInfo.compareEnable = 0;
    createInfo.maxAnisotropy = 8.0f;
    createInfo.unnormalizedCoordinates = 0;
    createInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    createInfo.minLod = 0;
    createInfo.maxLod = VK_LOD_CLAMP_NONE;
    createInfo.pNext = nullptr;

    VK_CHECK_RESULT( vkCreateSampler( device.getHandle(), &createInfo, nullptr, &m_Sampler ) )

    m_descriptorInfo.sampler = m_Sampler;
}

Sampler::~Sampler() {
    if( m_Sampler ) {
        vkDestroySampler( m_device.getHandle(), m_Sampler, nullptr );
    }
}
} // namespace onyx::rhi::vulkan
