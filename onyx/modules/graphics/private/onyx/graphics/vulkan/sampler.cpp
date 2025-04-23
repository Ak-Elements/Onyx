#include <onyx/graphics/vulkan/sampler.h>

#include <onyx/graphics/vulkan/device.h>

namespace Onyx::Graphics::Vulkan
{
    namespace
    {
        VkSamplerAddressMode ToVulkanAddressMode(SamplerAddressMode mode)
        {
            switch (mode)
            {
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
                ONYX_ASSERT(false, "Unhandeled sampler address mode %s", Enums::ToString(mode).data());
                return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
            }
        }

        VkFilter ToVulkanSamplerFilter(SamplerFilter mode)
        {
            switch (mode)
            {
                case SamplerFilter::Nearest:
                return VK_FILTER_NEAREST;
                case SamplerFilter::Linear:
                return VK_FILTER_LINEAR;
            case SamplerFilter::Cubic:
                return VK_FILTER_CUBIC_EXT;
            default:
                ONYX_ASSERT(false, "Unhandeled sampler filter mode %s", Enums::ToString(mode).data());
                return VK_FILTER_MAX_ENUM;
            }
        }

        VkSamplerMipmapMode ToVulkanSamplerMipMapMode(SamplerMipMapMode mode)
        {
            switch (mode)
            {
            case SamplerMipMapMode::Nearest:
                return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case SamplerMipMapMode::Linear:
                return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            default:
                ONYX_ASSERT(false, "Unhandeled sampler mipmap mode %s", Enums::ToString(mode).data());
                return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
            }
        }
    }

    Sampler::Sampler(const Device& device, const SamplerProperties& properties)
        : m_Device(device)
    {
        VkSamplerCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.addressModeU = ToVulkanAddressMode(properties.AddressModeU);
        createInfo.addressModeV = ToVulkanAddressMode(properties.AddressModeV);
        createInfo.addressModeW = ToVulkanAddressMode(properties.AddressModeW);
        createInfo.minFilter = ToVulkanSamplerFilter(properties.MinFilter);
        createInfo.magFilter = ToVulkanSamplerFilter(properties.MagFilter);
        createInfo.mipmapMode = ToVulkanSamplerMipMapMode(properties.MipFilter);
        createInfo.anisotropyEnable = 0;
        createInfo.compareEnable = 0;
        createInfo.maxAnisotropy = 1.0f;
        createInfo.unnormalizedCoordinates = 0;
        createInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        createInfo.minLod = 0;
        createInfo.maxLod = 16;
        createInfo.pNext = nullptr;

        VK_CHECK_RESULT(vkCreateSampler(device.GetHandle(), &createInfo, nullptr, &m_Sampler))
    }

    Sampler::~Sampler()
    {
        if (m_Sampler)
        {
            vkDestroySampler(m_Device.GetHandle(), m_Sampler, nullptr);
        }
    }
}
