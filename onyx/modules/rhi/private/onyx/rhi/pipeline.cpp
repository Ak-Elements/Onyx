#include <onyx/rhi/pipeline.h>

#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/shader/shader.h> // needed for destructor of PipelineProperties

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
bool Serialization< rhi::PipelineProperties >::serialize( Serializer& serializer,
                                                          const rhi::PipelineProperties& properties ) {
    return serializer.write< "depthstencil" >( properties.DepthStencil ) &&
           serializer.write< "rasterization" >( properties.Rasterization ) &&
           serializer.write< "blend" >( properties.BlendStates );
}

bool Serialization< rhi::PipelineProperties >::deserialize( const Deserializer& deserializer,
                                                            rhi::PipelineProperties& outProperties ) {
    return deserializer.readOptional< "depthstencil" >( outProperties.DepthStencil ) &&
           deserializer.readOptional< "rasterization" >( outProperties.Rasterization ) &&
           deserializer.readOptional< "blend" >( outProperties.BlendStates );
}

bool Serialization< rhi::Rasterization >::serialize( Serializer& serializer, const rhi::Rasterization& rasterization ) {
    return serializer.write< "cull" >( rasterization.CullMode ) &&
           serializer.write< "fill" >( rasterization.FillMode ) &&
           serializer.write< "frontfacing" >( rasterization.IsFrontFacing );
}

bool Serialization< rhi::Rasterization >::deserialize( const Deserializer& deserializer,
                                                       rhi::Rasterization& rasterization ) {
    return deserializer.readOptional< "cull" >( rasterization.CullMode ) &&
           deserializer.readOptional< "fill" >( rasterization.FillMode ) &&
           deserializer.readOptional< "frontfacing" >( rasterization.IsFrontFacing );
}

bool Serialization< rhi::DepthStencil >::serialize( Serializer& serializer, const rhi::DepthStencil& depthStencil ) {
    return serializer.write< "enabled" >( depthStencil.IsDepthEnabled ) &&
           serializer.write< "write" >( depthStencil.IsDepthWriteEnabled ) &&
           serializer.write< "stencil" >( depthStencil.IsStencilEnabled ) &&
           serializer.write< "compare" >( depthStencil.Compare );
}

bool Serialization< rhi::DepthStencil >::deserialize( const Deserializer& deserializer,
                                                      rhi::DepthStencil& outDepthStencil ) {
    return deserializer.readOptional< "enabled" >( outDepthStencil.IsDepthEnabled ) &&
           deserializer.readOptional< "write" >( outDepthStencil.IsDepthWriteEnabled ) &&
           deserializer.readOptional< "stencil" >( outDepthStencil.IsStencilEnabled ) &&
           deserializer.readOptional< "compare" >( outDepthStencil.Compare );
}

bool Serialization< rhi::BlendState >::serialize( Serializer& serializer, const rhi::BlendState& blendState ) {
    return serializer.write< "enabled" >( blendState.IsBlendEnabled ) &&
           serializer.write< "colormask" >( blendState.ColorWriteMask ) &&
           serializer.write< "srccolor" >( blendState.SourceColor ) &&
           serializer.write< "dstcolor" >( blendState.DestinationColor ) &&
           serializer.write< "colorop" >( blendState.ColorOperation ) &&
           serializer.write< "srcalpha" >( blendState.SourceAlpha ) &&
           serializer.write< "dstalpha" >( blendState.DestinationAlpha ) &&
           serializer.write< "alphaop" >( blendState.AlphaOperation );
}

bool Serialization< rhi::BlendState >::deserialize( const Deserializer& deserializer, rhi::BlendState& outBlendState ) {
    return deserializer.readOptional< "enabled" >( outBlendState.IsBlendEnabled ) &&
           deserializer.readOptional< "colormask" >( outBlendState.ColorWriteMask ) &&
           deserializer.readOptional< "srccolor" >( outBlendState.SourceColor ) &&
           deserializer.readOptional< "dstcolor" >( outBlendState.DestinationColor ) &&
           deserializer.readOptional< "colorop" >( outBlendState.ColorOperation ) &&
           deserializer.readOptional< "srcalpha" >( outBlendState.SourceAlpha ) &&
           deserializer.readOptional< "dstalpha" >( outBlendState.DestinationAlpha ) &&
           deserializer.readOptional< "alphaop" >( outBlendState.AlphaOperation );
}
} // namespace onyx