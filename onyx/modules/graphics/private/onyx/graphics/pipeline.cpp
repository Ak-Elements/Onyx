#include <onyx/graphics/pipeline.h>

#include <onyx/graphics/renderpass.h>
#include <onyx/graphics/shader/shadermodule.h> // needed for destructor of PipelineProperties

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    bool Serialization<Graphics::PipelineProperties>::Serialize(Serializer& serializer, const Graphics::PipelineProperties& properties)
    {
        return serializer.Write<"depthstencil">(properties.m_DepthStencil) &&
            serializer.Write<"rasterization">(properties.m_Rasterization) &&
            serializer.Write<"blend">(properties.BlendStates);
    }

    bool Serialization<Graphics::PipelineProperties>::Deserialize(const Deserializer& deserializer, Graphics::PipelineProperties& outProperties)
    {
        return deserializer.ReadOptional<"depthstencil">(outProperties.m_DepthStencil) &&
            deserializer.ReadOptional<"rasterization">(outProperties.m_Rasterization) &&
            deserializer.ReadOptional<"blend">(outProperties.BlendStates);
    }  

    bool Serialization<Graphics::Rasterization>::Serialize(Serializer& serializer, const Graphics::Rasterization& rasterization)
    {
        return serializer.Write<"cull">(rasterization.m_CullMode) &&
            serializer.Write<"fill">(rasterization.m_FillMode) &&
            serializer.Write<"frontfacing">(rasterization.IsFrontFacing);
    }

    bool Serialization<Graphics::Rasterization>::Deserialize(const Deserializer& deserializer, Graphics::Rasterization& rasterization)
    {
        return deserializer.ReadOptional<"cull">(rasterization.m_CullMode) &&
            deserializer.ReadOptional<"fill">(rasterization.m_FillMode) &&
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

namespace Onyx::Graphics
{
    PipelineProperties::~PipelineProperties() = default;
}