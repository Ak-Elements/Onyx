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
    StringId32 GetTypeId() const { return TypeId; }

    Reference< Buffer > Buffer;
    int8_t Alias = InvalidIndex8;

    operator bool() const { return Buffer.isValid(); }

    uint64_t GetGpuAddress() const { return Buffer->GetGpuAddress() + GetOffset(); }
    uint64_t GetOffset() const { return Alias == InvalidIndex8 ? 0 : Buffer->GetAliasOffset( Alias ); }
    uint64_t GetSize() const { return Buffer->GetAliasSize( Alias ); }

    template < typename T >
    void SetData( const T& data ) {
        Buffer->SetData( GetOffset(), &data, sizeof( T ) );
    }

    template < typename T >
    void SetData( Span< T > span ) {
        Buffer->SetData( static_cast< uint32_t >( GetOffset() ), span.data(), static_cast< int32_t >( span.size() * sizeof( T ) ) );
    }

    friend bool operator==( const BufferHandle& lhs, const BufferHandle& rhs ) {
        return lhs.Buffer == rhs.Buffer && lhs.Alias == rhs.Alias;
    }

    friend bool operator!=( const BufferHandle& lhs, const BufferHandle& rhs ) { return !( lhs == rhs ); }
};

struct TextureHandle {
    static constexpr StringId32 TypeId = "onyx::rhi::TextureHandle";
    StringId32 GetTypeId() const { return TypeId; }

    TextureStorageHandle Storage;
    TextureViewHandle Texture;
    int8_t Alias = InvalidIndex8;

    operator bool() const { return IsValid(); }

    bool IsValid() const { return Texture && Storage; }

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
    static bool serialize( file_system::JsonValue& json, const rhi::BufferHandle& handle ) {
        ONYX_UNUSED( json );
        ONYX_UNUSED( handle );
        return true;
    }

    static bool deserialize( const file_system::JsonValue& json, rhi::BufferHandle& handle ) {
        ONYX_UNUSED( json );
        ONYX_UNUSED( handle );
        return true;
    }
};

template <>
struct PinMetaObject< rhi::TextureHandle > {
#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, rhi::TextureHandle& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFB48246; /*Steel Blue*/ }
#endif

    static bool serialize( file_system::JsonValue& json, const rhi::TextureHandle& handle ) {
        ONYX_UNUSED( json );
        ONYX_UNUSED( handle );
        return true;
    }

    static bool deserialize( const file_system::JsonValue& json, rhi::TextureHandle& handle ) {
        ONYX_UNUSED( json );
        ONYX_UNUSED( handle );
        return true;
    }
};
} // namespace onyx::node_graph
