#pragma once

#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/textureproperties.h>

namespace onyx::rhi {
class GraphicsSystem;
class TextureStorage;

namespace vulkan {
class VulkanGraphicsApi;
}

namespace internal {
template < typename T >
struct TextureDeleter;
}

class Texture : public RefCounted {
    friend class vulkan::VulkanGraphicsApi;
    friend struct internal::TextureDeleter< Texture >;

  public:
    Texture( const TextureProperties& properties, const TextureStorage* storage );
    ~Texture() override = default;

    TextureProperties& getProperties() { return m_properties; }
    [[nodiscard]] const TextureProperties& getProperties() const { return m_properties; }

    [[nodiscard]] const TextureStorage& getStorage() const {
        ONYX_ASSERT( m_storage != nullptr );
        return *m_storage;
    }

    [[nodiscard]] GpuTextureAddress getGpuAddress() const { return m_gpuAddress; }

  private:
    void setGpuAddress( GpuTextureAddress address ) { m_gpuAddress = address; }

    virtual void release() = 0;

  protected:
    GpuTextureAddress m_gpuAddress{ std::numeric_limits< uint32_t >::max() };
    // index in memory pool and bindless texture index
    TextureProperties m_properties;
    const TextureStorage* m_storage; // non owning
};

namespace internal {
template < typename T >
struct TextureDeleter {
    TextureDeleter() = default;

    template < typename Ty2, std::enable_if_t< std::is_convertible_v< Ty2*, T* >, int > = 0 >
    constexpr TextureDeleter( const TextureDeleter< Ty2 >& ) noexcept {}

    void operator()( T* texture ) const {
        // if no index is set the texture is not from the bindless pool delete normally
        if( texture->getGpuAddress().isValid() ) {
            texture->release();
        } else {
            ONYX_SAFE_DELETE( texture );
        }
    }
};
} // namespace internal

using TextureDeleter = internal::TextureDeleter< Texture >;

namespace utils {
inline bool isDepthFormat( TextureFormat format ) {
    return ( format == TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT ) || ( format == TextureFormat::DEPTH_FLOAT32 ) ||
           ( format == TextureFormat::DEPTH_STENCIL_UNORM24_8UINT ) ||
           ( format == TextureFormat::DEPTH_STENCIL_UNORM16_8UINT ) || ( format == TextureFormat::DEPTH_UNORM16 ) ||
           ( format == TextureFormat::STENCIL_UINT8 ); // is this valid?
}

inline bool hasStencil( TextureFormat format ) {
    return ( format == TextureFormat::STENCIL_UINT8 ) || ( format == TextureFormat::DEPTH_STENCIL_UNORM16_8UINT ) ||
           ( format == TextureFormat::DEPTH_STENCIL_UNORM24_8UINT ) ||
           ( format == TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT );
}

inline uint32_t getImageFormatBpp( TextureFormat format ) {
    switch( format ) {
    case TextureFormat::R_UNORM8:
        return 1;
    case TextureFormat::R_UNORM16:
        return 2;
    case TextureFormat::R_UINT8:
        return 1;
    case TextureFormat::R_UINT16:
        return 2;
    case TextureFormat::R_UINT32:
        return 4;
    case TextureFormat::R_FLOAT32:
        return 4;
    case TextureFormat::RGB_UNORM8:
    case TextureFormat::SRGB_UNORM8:
        return 3;
    case TextureFormat::BGRA_UNORM8:
        return 4;
    case TextureFormat::RGBA_UNORM8:
        return 4;
    case TextureFormat::RG_FLOAT16:
        return 2 * 2;
    case TextureFormat::RGBA_UNORM16:
        return 4 * 2;
    case TextureFormat::RGBA_FLOAT16:
        return 2 * 4;
    case TextureFormat::RG_FLOAT32:
        return 2 * 4;
    case TextureFormat::RGB_FLOAT32:
        return 3 * 4;
    case TextureFormat::RGBA_FLOAT32:
        return 4 * 4;
    case TextureFormat::RGB_UFLOAT32_PACKED_11_11_10:
        return 4;

    case TextureFormat::Invalid:
    case TextureFormat::RG_UNORM8:
    case TextureFormat::DEPTH_UNORM16:
    case TextureFormat::DEPTH_FLOAT32:
    case TextureFormat::STENCIL_UINT8:
    case TextureFormat::DEPTH_STENCIL_UNORM16_8UINT:
    case TextureFormat::DEPTH_STENCIL_UNORM24_8UINT:
    case TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT: {
        ONYX_ASSERT( false, "Texture format not supported." );
        return 0;
    }
    }
    ONYX_ASSERT( false, "Texture format not implemented." );
    return 0;
}

inline bool isIntegerBased( const TextureFormat format ) {
    switch( format ) {
    case TextureFormat::R_UINT8:
    case TextureFormat::R_UINT16:
    case TextureFormat::R_UINT32:
    case TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT:
    case TextureFormat::STENCIL_UINT8:
        return true;
    case TextureFormat::R_UNORM8:
    case TextureFormat::R_UNORM16:
    case TextureFormat::RGB_UFLOAT32_PACKED_11_11_10:
    case TextureFormat::RG_FLOAT16:
    case TextureFormat::RG_FLOAT32:
    case TextureFormat::R_FLOAT32:
    case TextureFormat::RG_UNORM8:
    case TextureFormat::RGB_FLOAT32:
    case TextureFormat::RGBA_FLOAT32:
    case TextureFormat::RGBA_FLOAT16:
    case TextureFormat::RGB_UNORM8:
    case TextureFormat::RGBA_UNORM8:
    case TextureFormat::BGRA_UNORM8:
    case TextureFormat::SRGB_UNORM8:
    case TextureFormat::RGBA_UNORM16:
    case TextureFormat::DEPTH_UNORM16:
    case TextureFormat::DEPTH_FLOAT32:
    case TextureFormat::DEPTH_STENCIL_UNORM24_8UINT:
    case TextureFormat::DEPTH_STENCIL_UNORM16_8UINT:
        return false;

    case TextureFormat::Invalid: {
        ONYX_ASSERT( false, "Texture format not supported." );
        return false;
    }
    }

    ONYX_ASSERT( false, "Texture format not implemented." );
    return false;
}

inline uint32_t calculateMipCount( uint32_t width, uint32_t height ) {
    return static_cast< uint32_t >( std::log2( std::min( width, height ) ) ) + 1;
}

inline uint32_t getImageMemorySize( TextureFormat format, uint32_t width, uint32_t height ) {
    return width * height * getImageFormatBpp( format );
}

} // namespace utils
} // namespace onyx::rhi
