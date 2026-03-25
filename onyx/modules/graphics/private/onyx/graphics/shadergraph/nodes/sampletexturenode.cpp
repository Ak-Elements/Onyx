#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
SampleTextureNode::SampleTextureNode() {
    AddInPin< TextureInPin >();
    AddInPin< UVInPin >();

    AddOutPin< RGBOutPin >();
    AddOutPin< RGBAOutPin >();
    AddOutPin< RedOutPin >();
    AddOutPin< GreenOutPin >();
    AddOutPin< BlueOutPin >();
    AddOutPin< AlphaOutPin >();
}

SampleTextureNode::~SampleTextureNode() = default;

void SampleTextureNode::OnUpdate( node_graph::ExecutionContext& context ) const {
    const TextureInPin& inputPin = static_cast< const TextureInPin& >( *GetInputPin( 0 ) );
    if ( ( inputPin.IsConnected() == false ) && Texture.isValid() && Texture->isLoaded() ) {
        ShaderGraphTextures& textures = context.Get< ShaderGraphTextures >();
        textures.AddTexture( Texture->GetTextureHandle() );
    }
}

bool SampleTextureNode::OnSerialize( Serializer& serializer ) const {
    if ( Texture.hasAssetId() ) {
        // TODO: Add asset id serializer
        serializer.write< "sampleTextureId" >( Texture.getId().get() );
    }

    return FlexiblePinsNode::OnSerialize( serializer );
}

bool SampleTextureNode::OnDeserialize( const Deserializer& deserializer ) {
    uint64_t assetId;
    if ( deserializer.read< "sampleTextureId" >( assetId ) ) {
        Texture.setId( assets::AssetId( assetId ) );
    }

    return FlexiblePinsNode::OnDeserialize( deserializer );
}

void SampleTextureNode::DoGenerateShader( const node_graph::ExecutionContext& context,
                                          rhi::ShaderGenerator& generator ) const {
    if ( generator.GetStage() != rhi::ShaderStage::Fragment )
        return;

    const TextureInPin& inputPin = static_cast< const TextureInPin& >( *GetInputPin( 0 ) );

    int32_t textureIndex;
    // const ShaderGraphTextures& shaderGraphTextures = context.Get<ShaderGraphTextures>();
    if ( inputPin.IsConnected() ) {
        textureIndex = generator.GetTextureIndex( inputPin.GetLinkedPinGlobalId().get() );
    } else {
        // TODO: we need to get the texture from the asset here to store it similar to OnUpdate and avoid adding
        // duplicates
        // shaderGraphTextures.GetTextureEntryIndex
        textureIndex = generator.AddTexture( Texture.getId().get() );
    }

    if ( textureIndex == InvalidIndex32 ) {
        ONYX_LOG_WARNING( "Missing texture for texture sample node ({:x})", GetId().get() );
        // TODO: return bool
    }

    // Texture input
    const uint64_t texturePinGlobalId = inputPin.GetGlobalId().get();
    const String textureSampleVariable = format::format( "sampledTexture_{:x}", texturePinGlobalId );

    // UV Input
    const UVInPin& uvInputPin = static_cast< const UVInPin& >( *GetInputPin( 1 ) );
    const String textureCoordsString = uvInputPin.IsConnected()
                                           ? format::format( "pin_{:x}", uvInputPin.GetLinkedPinGlobalId().get() )
                                           : rhi::ShaderGenerator::GenerateShaderValue(
                                                 context.GetPinData< UVInPin >() );

    // Sampling code
    String textureSampleCode = format::format(
        "vec4 {} = texture(BindlessTextures[nonuniformEXT(TextureIndices[{}])], {}); \n",
        textureSampleVariable,
        textureIndex,
        textureCoordsString );

    // Outputs
    bool isAnyOutPinConnected = false;
    Optional< const RGBOutPin* > rgbOutputPin = GetOutputPinByLocalId< RGBOutPin >();
    if ( rgbOutputPin && context.IsPinConnected< RGBOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "vec3 pin_{:x} = {}.xyz; // rgb \n",
                                             rgbOutputPin.value()->GetGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const RGBAOutPin* > rgbaOutputPin = GetOutputPinByLocalId< RGBAOutPin >();
    if ( rgbaOutputPin && context.IsPinConnected< RGBAOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "vec4 pin_{:x} = {}.xyzw; // rgba \n",
                                             rgbaOutputPin.value()->GetGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const RedOutPin* > redOutputPin = GetOutputPinByLocalId< RedOutPin >();
    if ( redOutputPin && context.IsPinConnected< RedOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.x; // red \n",
                                             redOutputPin.value()->GetGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const GreenOutPin* > greenOutputPin = GetOutputPinByLocalId< GreenOutPin >();
    if ( greenOutputPin && context.IsPinConnected< GreenOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.y; // green \n",
                                             greenOutputPin.value()->GetGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const BlueOutPin* > blueOutputPin = GetOutputPinByLocalId< BlueOutPin >();
    if ( blueOutputPin && context.IsPinConnected< BlueOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.z; // blue \n",
                                             blueOutputPin.value()->GetGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const AlphaOutPin* > alphaOutputPin = GetOutputPinByLocalId< AlphaOutPin >();
    if ( alphaOutputPin && context.IsPinConnected< AlphaOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.w; // alpha \n",
                                             alphaOutputPin.value()->GetGlobalId().get(),
                                             textureSampleVariable );
    }

    if ( isAnyOutPinConnected ) {
        generator.AppendCode( textureSampleCode );
    }
}

void SampleTextureNode::OnChanged( assets::AssetSystem& assetSystem ) {
    if ( Texture.hasAssetId() ) {
        if ( Texture.isValid() == false ) {
            assetSystem.getAsset( Texture.getId(), Texture );
        }
    } else {
        Texture.reset();
    }
}

#if ONYX_IS_EDITOR
StringView SampleTextureNode::GetPinName( StringId32 pinId ) const {
    switch ( pinId ) {
    case TextureInPin::LocalId:
        return "Texture";
    case UVInPin::LocalId:
        return "UVs";
    case RGBOutPin::LocalId:
        return "RGB";
    case RGBAOutPin::LocalId:
        return "RGBA";
    case RedOutPin::LocalId:
        return "R";
    case GreenOutPin::LocalId:
        return "G";
    case BlueOutPin::LocalId:
        return "B";
    case AlphaOutPin::LocalId:
        return "A";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

node_graph::PinVisibility SampleTextureNode::DoGetPinVisibility( StringId32 localPinId ) const {
    switch ( localPinId ) {
    case TextureInPin::LocalId:
        return node_graph::PinVisibility::InNode;
    default:
        return node_graph::PinVisibility::Default;
    }
}
#endif

} // namespace onyx::graphics::shader_graph_nodes
