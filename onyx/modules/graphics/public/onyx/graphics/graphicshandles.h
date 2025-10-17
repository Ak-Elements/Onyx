#pragma once

#include <onyx/graphics/texture.h>
#include <onyx/graphics/buffer.h>
#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/graphics/shader/shadereffect.h>
#include <onyx/graphics/pipeline.h>
#include <onyx/graphics/texturestorage.h>
#include <onyx/graphics/framebuffer.h>
#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/descriptorset.h>

#include <onyx/nodegraph/pins/pinmeta.h>

namespace Onyx::FileSystem
{
    struct JsonValue;
}

namespace Onyx::Graphics
{
    //struct TextureHandle;
    //class DescriptorSet;
    //class Framebuffer;
    //class Buffer;
    //class Texture;
    //class TextureStorage;
    //class RenderPass;
    //class Pipeline;
    //class Shader;
    //class ShaderEffect;

    //using BufferHandle = Reference<Buffer>;
    using TextureViewHandle = Reference<Texture, TextureDeleter>;
    using TextureStorageHandle = Reference<TextureStorage>;
    using ShaderHandle = Reference<Shader>;
    using ShaderEffectHandle = Reference<ShaderEffect>;
    using RenderPassHandle = Reference<RenderPass>;
    using FramebufferHandle = Reference<Framebuffer>;
    using PipelineHandle = Reference<Pipeline>;
    using DescriptorSetHandle = Reference<DescriptorSet>;

    struct BufferHandle
    {
        Reference<Buffer> Buffer;
        onyxS8 Alias = INVALID_INDEX_8;

        operator bool() const
        {
            return Buffer;
        }

        onyxU64 GetGpuAddress() const { return Buffer->GetGpuAddress() + GetOffset(); }
        onyxU64 GetOffset() const { return Alias == INVALID_INDEX_8 ? 0 : Buffer->GetAliasOffset(Alias); }
        onyxU64 GetSize() const { return Buffer->GetAliasSize(Alias); }

        friend bool operator==(const BufferHandle& lhs, const BufferHandle& rhs)
        {
            return lhs.Buffer == rhs.Buffer && lhs.Alias == rhs.Alias;
        }

        friend bool operator!=(const BufferHandle& lhs, const BufferHandle& rhs)
        {
            return !(lhs == rhs);
        }
    };

    struct TextureHandle
    {
        static constexpr StringId32 TypeId = "Onyx::Graphics::TextureHandle";
        StringId32 GetTypeId() const { return TypeId; }

        TextureStorageHandle Storage;
        TextureViewHandle Texture;
        onyxS8 Alias = INVALID_INDEX_8;

        operator bool() const
        {
            return Texture && Storage;
        }

        friend bool operator==(const TextureHandle& lhs, const TextureHandle& rhs)
        {
            return lhs.Texture == rhs.Texture && lhs.Storage == rhs.Storage;
        }

        friend bool operator!=(const TextureHandle& lhs, const TextureHandle& rhs)
        {
            return !(lhs == rhs);
        }
    };
}

namespace Onyx
{
    template<>
    struct Serialization<Graphics::TextureHandle>
    {
        static bool Serialize(Serializer&, const Graphics::TextureHandle&)
        {
            // not supported but needed for the graph pins
            return true;
        }

        static bool Deserialize(const Deserializer&, Graphics::TextureHandle&)
        {
            // not supported but needed for the graph pins
            return true;
        }
    };

    template<>
    struct Serialization<Graphics::BufferHandle>
    {
        static bool Serialize(Serializer&, const Graphics::BufferHandle&)
        {
            // not supported but needed for the graph pins
            return true;
        }

        static bool Deserialize(const Deserializer&, Graphics::BufferHandle&)
        {
            // not supported but needed for the graph pins
            return true;
        }
    };
}

namespace Onyx::NodeGraph
{
    template <>
    struct PinMetaObject<Graphics::BufferHandle>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Graphics::BufferHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF5C5CCD; /* Indian Red */ }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Graphics::BufferHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Graphics::BufferHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Graphics::TextureHandle>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Graphics::TextureHandle& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFB48246; /*Steel Blue*/ }
#endif

        static bool Serialize(FileSystem::JsonValue& json, const Graphics::TextureHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Graphics::TextureHandle& handle)
        {
            ONYX_UNUSED(json);
            ONYX_UNUSED(handle);
            return true;
        }
    };
}
