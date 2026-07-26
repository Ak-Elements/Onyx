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
    const VulkanTextureStorage* textureStorage = static_cast< const VulkanTextureStorage* >( m_Storage );

    const VkImageAspectFlags aspectMask = VulkanTextureStorage::GetAspectFlags( m_Properties.Format );

    const TextureStorageProperties& storageProperties = m_Storage->GetProperties();
    const uint32_t storageArraySize = std::max< uint16_t >( storageProperties.m_ArraySize, 1u );
    const uint32_t arraySize = m_Properties.ArraySize ? m_Properties.ArraySize
                                                      : storageArraySize - m_Properties.ArrayIndex;
    ONYX_ASSERT( arraySize <= storageArraySize );

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = ( aliasIndex == InvalidIndex8 ) ? textureStorage->GetHandle()
                                                                : textureStorage->GetAliasHandle( aliasIndex );
    imageViewCreateInfo.pNext = nullptr;

    const uint32_t mip = m_Properties.MipLevel;
    const uint32_t miplevels = m_Properties.MaxMipLevel ? m_Properties.MaxMipLevel
                                                        : storageProperties.m_MaxMipLevel - mip;
    ONYX_ASSERT( mip + miplevels <= storageProperties.m_MaxMipLevel );

    imageViewCreateInfo.format = VulkanTextureStorage::GetFormat( m_Properties.Format );
    imageViewCreateInfo.components = VkComponentMapping{ VK_COMPONENT_SWIZZLE_R,
                                                         VK_COMPONENT_SWIZZLE_G,
                                                         VK_COMPONENT_SWIZZLE_B,
                                                         VK_COMPONENT_SWIZZLE_A };
    imageViewCreateInfo.subresourceRange = VkImageSubresourceRange{ aspectMask,
                                                                    mip,
                                                                    miplevels,
                                                                    m_Properties.ArrayIndex,
                                                                    arraySize };

    if( storageProperties.m_Type == TextureType::TextureCube ) {
        imageViewCreateInfo.subresourceRange.layerCount *= 6;
        imageViewCreateInfo.viewType = m_Properties.IsWriteable || m_Properties.AllowCubeMapLoads
                                           ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
                                           : VK_IMAGE_VIEW_TYPE_CUBE;
    } else
        imageViewCreateInfo.viewType = getType( storageProperties.m_Type, storageProperties.m_ArraySize != 0 );

    VK_CHECK_RESULT( vkCreateImageView( device.GetHandle(), &imageViewCreateInfo, nullptr, &m_ImageView ) )
    SetResourceName( device.GetHandle(),
                     VK_OBJECT_TYPE_IMAGE_VIEW,
                     (uint64_t)m_ImageView,
                     m_Properties.DebugName.empty() ? "Unnamed Texture" : m_Properties.DebugName.c_str() );

    if( Utils::IsDepthFormat( m_Properties.Format ) )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    else if( m_Properties.Usage == TextureUsage::Storage )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    else
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if( m_Properties.Usage == TextureUsage::Storage )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    else if( m_Properties.Usage == TextureUsage::HostRead )
        m_descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    m_descriptorInfo.imageView = m_ImageView;
    if( m_Properties.Sampler.has_value() ) {
        Reference< rhi::Sampler > sampler = m_api->getSampler( *m_Properties.Sampler );
        m_descriptorInfo.sampler = sampler.as< Sampler >().GetHandle();
    }
}

VulkanTexture::~VulkanTexture() {
    if( m_ImageView != nullptr ) {
        vkDestroyImageView( m_api->getDevice().GetHandle(), m_ImageView, nullptr );
        m_ImageView = nullptr;
    }
}

void VulkanTexture::Release() {
    m_api->releaseTexture( *this );
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
