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

    using BufferHandle = Reference<Buffer>;
    using TextureViewHandle = Reference<Texture, TextureDeleter>;
    using TextureStorageHandle = Reference<TextureStorage>;
    using ShaderHandle = Reference<Shader>;
    using ShaderEffectHandle = Reference<ShaderEffect>;
    using RenderPassHandle = Reference<RenderPass>;
    using FramebufferHandle = Reference<Framebuffer>;
    using PipelineHandle = Reference<Pipeline>;
    using DescriptorSetHandle = Reference<DescriptorSet>;

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
