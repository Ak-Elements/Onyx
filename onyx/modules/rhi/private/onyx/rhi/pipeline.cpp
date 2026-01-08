#include <onyx/rhi/pipeline.h>

#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/shader/shader.h> // needed for destructor of PipelineProperties

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    bool Serialization<Graphics::PipelineProperties>::Serialize(Serializer& serializer, const Graphics::PipelineProperties& properties)
    {
        return serializer.Write<"depthstencil">(properties.DepthStencil) &&
            serializer.Write<"rasterization">(properties.Rasterization) &&
            serializer.Write<"blend">(properties.BlendStates);
    }

    bool Serialization<Graphics::PipelineProperties>::Deserialize(const Deserializer& deserializer, Graphics::PipelineProperties& outProperties)
    {
        return deserializer.ReadOptional<"depthstencil">(outProperties.DepthStencil) &&
            deserializer.ReadOptional<"rasterization">(outProperties.Rasterization) &&
            deserializer.ReadOptional<"blend">(outProperties.BlendStates);
    }

    bool Serialization<Graphics::Rasterization>::Serialize(Serializer& serializer, const Graphics::Rasterization& rasterization)
    {
        return serializer.Write<"cull">(rasterization.CullMode) &&
            serializer.Write<"fill">(rasterization.FillMode) &&
            serializer.Write<"frontfacing">(rasterization.IsFrontFacing);
    }

    bool Serialization<Graphics::Rasterization>::Deserialize(const Deserializer& deserializer, Graphics::Rasterization& rasterization)
    {
        return deserializer.ReadOptional<"cull">(rasterization.CullMode) &&
            deserializer.ReadOptional<"fill">(rasterization.FillMode) &&
            deserializer.ReadOptional<"frontfacing">(rasterization.IsFrontFacing);
    }

    bool Serialization<Graphics::DepthStencil>::Serialize(Serializer& serializer, const Graphics::DepthStencil& depthStencil)
    {
        return serializer.Write<"enabled">(depthStencil.IsDepthEnabled) &&
            serializer.Write<"write">(depthStencil.IsDepthWriteEnabled) &&
            serializer.Write<"stencil">(depthStencil.IsStencilEnabled) &&
            serializer.Write<"compare">(depthStencil.Compare);
    }

    bool Serialization<Graphics::DepthStencil>::Deserialize(const Deserializer& deserializer, Graphics::DepthStencil& outDepthStencil)
    {
        return deserializer.ReadOptional<"enabled">(outDepthStencil.IsDepthEnabled) &&
            deserializer.ReadOptional<"write">(outDepthStencil.IsDepthWriteEnabled) &&
            deserializer.ReadOptional<"stencil">(outDepthStencil.IsStencilEnabled) &&
            deserializer.ReadOptional<"compare">(outDepthStencil.Compare);
    }

    bool Serialization<Graphics::BlendState>::Serialize(Serializer& serializer, const Graphics::BlendState& blendState)
    {
        return serializer.Write<"enabled">(blendState.IsBlendEnabled) &&
            serializer.Write<"colormask">(blendState.ColorWriteMask) &&
            serializer.Write<"srccolor">(blendState.SourceColor) &&
            serializer.Write<"dstcolor">(blendState.DestinationColor) &&
            serializer.Write<"colorop">(blendState.ColorOperation) &&
            serializer.Write<"srcalpha">(blendState.SourceAlpha) &&
            serializer.Write<"dstalpha">(blendState.DestinationAlpha) &&
            serializer.Write<"alphaop">(blendState.AlphaOperation);
    }

    bool Serialization<Graphics::BlendState>::Deserialize(const Deserializer& deserializer, Graphics::BlendState& outBlendState)
    {
        return deserializer.ReadOptional<"enabled">(outBlendState.IsBlendEnabled) &&
            deserializer.ReadOptional<"colormask">(outBlendState.ColorWriteMask) &&
            deserializer.ReadOptional<"srccolor">(outBlendState.SourceColor) &&
            deserializer.ReadOptional<"dstcolor">(outBlendState.DestinationColor) &&
            deserializer.ReadOptional<"colorop">(outBlendState.ColorOperation) &&
            deserializer.ReadOptional<"srcalpha">(outBlendState.SourceAlpha) &&
            deserializer.ReadOptional<"dstalpha">(outBlendState.DestinationAlpha) &&
            deserializer.ReadOptional<"alphaop">(outBlendState.AlphaOperation);
    }
}