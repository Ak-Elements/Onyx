#include <onyx/rhi/vulkan/texture.h>

#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/sampler.h>
#include <onyx/rhi/vulkan/texturestorage.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {

VulkanTexture::VulkanTexture( VulkanGraphicsApi& api,
                              const TextureProperties& properties,
                              const VulkanTextureStorage* storage )
    : VulkanTexture( api, properties, storage, InvalidIndex8 ) {}

VulkanTexture::VulkanTexture( VulkanGraphicsApi& api,
                              const TextureProperties& properties,
                              const VulkanTextureStorage* storage,
                              int8_t aliasIndex )
    : Texture( properties, storage )
    , m_api( &api ) {
    init( api, aliasIndex );
}

void VulkanTexture::init( const VulkanGraphicsApi& api, int8_t aliasIndex ) {
    const Device& device = api.getDevice();
    const VulkanTextureStorage* textureStorage = static_cast< const VulkanTextureStorage* >( m_storage );

    const VkImageAspectFlags aspectMask = VulkanTextureStorage::getAspectFlags( m_properties.Format );

    const TextureStorageProperties& storageProperties = m_storage->getProperties();
    const uint32_t storageArraySize = std::max< uint16_t >( storageProperties.ArraySize, 1u );
    const uint32_t arraySize = m_properties.ArraySize ? m_properties.ArraySize
                                                      : storageArraySize - m_properties.ArrayIndex;
    ONYX_ASSERT( arraySize <= storageArraySize );

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = ( aliasIndex == InvalidIndex8 ) ? textureStorage->GetHandle()
                                                                : textureStorage->getAliasHandle( aliasIndex );
    imageViewCreateInfo.pNext = nullptr;

    const uint32_t mip = m_properties.MipLevel;
    const uint32_t miplevels = m_properties.MaxMipLevel ? m_properties.MaxMipLevel
                                                        : storageProperties.MaxMipLevel - mip;
    ONYX_ASSERT( mip + miplevels <= storageProperties.MaxMipLevel );

    imageViewCreateInfo.format = VulkanTextureStorage::getFormat( m_properties.Format );
    imageViewCreateInfo.components = VkComponentMapping{ VK_COMPONENT_SWIZZLE_R,
                                                         VK_COMPONENT_SWIZZLE_G,
                                                         VK_COMPONENT_SWIZZLE_B,
                                                         VK_COMPONENT_SWIZZLE_A };
    imageViewCreateInfo.subresourceRange = VkImageSubresourceRange{ aspectMask,
                                                                    mip,
                                                                    miplevels,
                                                                    m_properties.ArrayIndex,
                                                                    arraySize };

    if( storageProperties.Type == TextureType::TextureCube ) {
        imageViewCreateInfo.subresourceRange.layerCount *= 6;
        imageViewCreateInfo.viewType = m_properties.IsWriteable || m_properties.AllowCubeMapLoads
                                           ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
                                           : VK_IMAGE_VIEW_TYPE_CUBE;
    } else
        imageViewCreateInfo.viewType = getType( storageProperties.Type, storageProperties.ArraySize != 0 );

    VK_CHECK_RESULT( vkCreateImageView( device.GetHandle(), &imageViewCreateInfo, nullptr, &m_ImageView ) )
    SetResourceName( device.GetHandle(),
                     VK_OBJECT_TYPE_IMAGE_VIEW,
                     (uint64_t)m_ImageView,
                     m_properties.DebugName.empty() ? "Unnamed Texture" : m_properties.DebugName.c_str() );

    if( utils::isDepthFormat( m_properties.Format ) )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    else if( m_properties.Usage == TextureUsage::Storage )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    else
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if( m_properties.Usage == TextureUsage::Storage )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    else if( m_properties.Usage == TextureUsage::HostRead )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    m_descriptorInfo.imageView = m_ImageView;
    if( m_properties.Sampler.has_value() ) {
        Reference< rhi::Sampler > sampler = m_api->getSampler( *m_properties.Sampler );
        m_descriptorInfo.sampler = sampler.as< Sampler >().GetHandle();
    }
}

VulkanTexture::~VulkanTexture() {
    if( m_ImageView != nullptr ) {
        vkDestroyImageView( m_api->getDevice().GetHandle(), m_ImageView, nullptr );
        m_ImageView = nullptr;
    }
}

void VulkanTexture::release() {
    m_api->releaseTexture( *this );
    m_gpuAddress.reset();
}

VkImageViewType VulkanTexture::getType( TextureType type, bool isArray ) {
    switch( type ) {
    case TextureType::Texture1D:
        return isArray ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
    case TextureType::Texture2D:
        return isArray ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
    case TextureType::Texture3D:
        ONYX_ASSERT( !isArray, "3D texture does not support array texture view" );
        return VK_IMAGE_VIEW_TYPE_3D;
    case TextureType::TextureCube:
        return isArray ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
    default:
        ONYX_ASSERT( false, "Unknown storage type: {}, {}", enums::toString( type ), isArray );
        return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
}

} // namespace onyx::rhi::vulkan
