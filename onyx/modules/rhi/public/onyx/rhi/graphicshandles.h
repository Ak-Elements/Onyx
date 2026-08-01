#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/rhi/buffer.h>
#include <onyx/rhi/descriptorset.h>
#include <onyx/rhi/framebuffer.h>
#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/shader/shader.h>
#include <onyx/rhi/shader/shaderinstance.h>
#include <onyx/rhi/texture.h>
#include <onyx/rhi/texturestorage.h>

namespace onyx::file_system {
struct JsonValue;
}

namespace onyx::rhi {
// struct TextureHandle;
// class DescriptorSet;
// class Framebuffer;
// class Buffer;
// class Texture;
// class TextureStorage;
// class RenderPass;
// class Pipeline;
// class Shader;
// class ShaderEffect;

// using BufferHandle = Reference<Buffer>;
using TextureViewHandle = Reference< Texture, TextureDeleter >;
using TextureStorageHandle = Reference< TextureStorage >;
using ShaderHandle = assets::AssetHandle< Shader >;
using ShaderInstanceHandle = Reference< ShaderInstance >;
using RenderPassHandle = Reference< RenderPass >;
using FramebufferHandle = Reference< Framebuffer >;
using PipelineHandle = Reference< Pipeline >;
using DescriptorSetHandle = Reference< DescriptorSet >;

struct BufferHandle {
    static constexpr StringId32 TypeId = "onyx::rhi::BufferHandle";
    static constexpr StringId32 getTypeId() { return TypeId; }

    Reference< Buffer > Buffer;
    int8_t Alias = InvalidIndex8;

    bool isValid() const { return Buffer.isValid(); }
    operator bool() const { return isValid(); }

    uint64_t getGpuAddress() const {
        ONYX_ASSERT( isValid(), "Buffer is not valid" );
        ONYX_ASSERT( Buffer->hasGpuAddress(), "Buffer was not created with DeviceAddress flag." );
        return Buffer->getGpuAddress() + getOffset();
    }
    uint64_t getOffset() const { return Alias == InvalidIndex8 ? 0 : Buffer->getAliasOffset( Alias ); }
    uint64_t getSize() const { return Buffer->getAliasSize( Alias ); }

    template < typename T >
    void setData( const T& data ) {
        Buffer->setData( getOffset(), &data, sizeof( T ) );
    }

    template < typename T >
    void setData( Span< T > span ) {
        Buffer->setData( static_cast< uint32_t >( getOffset() ),
                         span.data(),
                         static_cast< int32_t >( span.size() * sizeof( T ) ) );
    }

    friend bool operator==( const BufferHandle& lhs, const BufferHandle& rhs ) {
        return lhs.Buffer == rhs.Buffer && lhs.Alias == rhs.Alias;
    }

    friend bool operator!=( const BufferHandle& lhs, const BufferHandle& rhs ) { return !( lhs == rhs ); }
};

struct TextureHandle {
    static constexpr StringId32 TypeId = "onyx::rhi::TextureHandle";
    static constexpr StringId32 getTypeId() { return TypeId; }

    TextureStorageHandle Storage;
    TextureViewHandle Texture;
    int8_t Alias = InvalidIndex8;

    operator bool() const { return isValid(); }

    bool isValid() const { return Texture && Storage; }

    friend bool operator==( const TextureHandle& lhs, const TextureHandle& rhs ) {
        return lhs.Texture == rhs.Texture && lhs.Storage == rhs.Storage;
    }

    friend bool operator!=( const TextureHandle& lhs, const TextureHandle& rhs ) { return !( lhs == rhs ); }
};
} // namespace onyx::rhi

namespace onyx {
template <>
struct Serialization< rhi::TextureHandle > {
    static bool serialize( Serializer&, const rhi::TextureHandle& ) {
        // not supported but needed for the graph pins
        return true;
    }

    static bool deserialize( const Deserializer&, rhi::TextureHandle& ) {
        // not supported but needed for the graph pins
        return true;
    }
};

template <>
struct Serialization< rhi::BufferHandle > {
    static bool serialize( Serializer&, const rhi::BufferHandle& ) {
        // not supported but needed for the graph pins
        return true;
    }

    static bool deserialize( const Deserializer&, rhi::BufferHandle& ) {
        // not supported but needed for the graph pins
        return true;
    }
};
} // namespace onyx

namespace onyx::node_graph {
template < typename T >
struct PinMetaObject;

template <>
struct PinMetaObject< rhi::BufferHandle > {
#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, rhi::BufferHandle& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF5C5CCD; /* Indian Red */ }
#endif
    static bool serialize( [[maybe_unused]] file_system::JsonValue& json,
                           [[maybe_unused]] const rhi::BufferHandle& handle ) {
        return true;
    }

    static bool deserialize( [[maybe_unused]] const file_system::JsonValue& json,
                             [[maybe_unused]] rhi::BufferHandle& handle ) {
        return true;
    }
};

template <>
struct PinMetaObject< rhi::TextureHandle > {
#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, rhi::TextureHandle& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFB48246; /*Steel Blue*/ }
#endif

    static bool serialize( [[maybe_unused]] file_system::JsonValue& json,
                           [[maybe_unused]] const rhi::TextureHandle& handle ) {
        return true;
    }

    static bool deserialize( [[maybe_unused]] const file_system::JsonValue& json,
                             [[maybe_unused]] rhi::TextureHandle& handle ) {
        return true;
    }
};
} // namespace onyx::node_graph
