#include <onyx/graphics/shadergraph/nodes/sampletexturenode.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
SampleTextureNode::SampleTextureNode() {
    addInPin< TextureInPin >();
    addInPin< UVInPin >();

    addOutPin< RGBOutPin >();
    addOutPin< RGBAOutPin >();
    addOutPin< RedOutPin >();
    addOutPin< GreenOutPin >();
    addOutPin< BlueOutPin >();
    addOutPin< AlphaOutPin >();
}

SampleTextureNode::~SampleTextureNode() = default;

void SampleTextureNode::onUpdate( node_graph::ExecutionContext& context ) const {
    const TextureInPin& inputPin = static_cast< const TextureInPin& >( *getInputPin( 0 ) );
    if( ( inputPin.isConnected() == false ) && m_texture.isValid() && m_texture->isLoaded() ) {
        ShaderGraphTextures& textures = context.get< ShaderGraphTextures >();
        textures.addTexture( m_texture->getTextureHandle() );
    }
}

bool SampleTextureNode::onSerialize( Serializer& serializer ) const {
    if( m_texture.hasAssetId() ) {
        // TODO: Add asset id serializer
        serializer.write< "sampleTextureId" >( m_texture.getId().get() );
    }

    return FlexiblePinsNode::onSerialize( serializer );
}

bool SampleTextureNode::onDeserialize( const Deserializer& deserializer ) {
    assets::AssetId assetId;
    if( deserializer.read< "sampleTextureId" >( assetId ) ) {
        m_texture.setId( assetId );
    }

    return FlexiblePinsNode::onDeserialize( deserializer );
}

void SampleTextureNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                          rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    const TextureInPin& inputPin = static_cast< const TextureInPin& >( *getInputPin( 0 ) );

    int32_t textureIndex;
    // const ShaderGraphTextures& shaderGraphTextures = context.Get<ShaderGraphTextures>();
    if( inputPin.isConnected() ) {
        textureIndex = generator.getTextureIndex( inputPin.getLinkedPinGlobalId().get() );
    } else {
        // TODO: we need to get the texture from the asset here to store it similar to OnUpdate and avoid adding
        // duplicates
        // shaderGraphTextures.GetTextureEntryIndex
        textureIndex = generator.addTexture( m_texture.getId().asUint64() );
    }

    if( textureIndex == InvalidIndex32 ) {
        ONYX_LOG_WARNING( "Missing texture for texture sample node ({:x})", getId().get() );
        // TODO: return bool
    }

    // Texture input
    const uint64_t texturePinGlobalId = inputPin.getGlobalId().get();
    const String textureSampleVariable = format::format( "sampledTexture_{:x}", texturePinGlobalId );

    // UV Input
    const UVInPin& uvInputPin = static_cast< const UVInPin& >( *getInputPin( 1 ) );
    const String textureCoordsString = uvInputPin.isConnected()
                                           ? format::format( "pin_{:x}", uvInputPin.getLinkedPinGlobalId().get() )
                                           : rhi::ShaderGenerator::generateShaderValue(
                                                 context.getPinData< UVInPin >() );

    // Sampling code
    String textureSampleCode = format::format( "float4 {} = linearSample( TextureIndices[ {} ] ) ], {} ); \n",
                                               textureSampleVariable,
                                               textureIndex,
                                               textureCoordsString );

    // Outputs
    bool isAnyOutPinConnected = false;
    Optional< const RGBOutPin* > rgbOutputPin = getOutputPinByLocalId< RGBOutPin >();
    if( rgbOutputPin && context.isPinConnected< RGBOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float3 pin_{:x} = {}.xyz; // rgb \n",
                                             rgbOutputPin.value()->getGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const RGBAOutPin* > rgbaOutputPin = getOutputPinByLocalId< RGBAOutPin >();
    if( rgbaOutputPin && context.isPinConnected< RGBAOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float4 pin_{:x} = {}.xyzw; // rgba \n",
                                             rgbaOutputPin.value()->getGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const RedOutPin* > redOutputPin = getOutputPinByLocalId< RedOutPin >();
    if( redOutputPin && context.isPinConnected< RedOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.x; // red \n",
                                             redOutputPin.value()->getGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const GreenOutPin* > greenOutputPin = getOutputPinByLocalId< GreenOutPin >();
    if( greenOutputPin && context.isPinConnected< GreenOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.y; // green \n",
                                             greenOutputPin.value()->getGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const BlueOutPin* > blueOutputPin = getOutputPinByLocalId< BlueOutPin >();
    if( blueOutputPin && context.isPinConnected< BlueOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.z; // blue \n",
                                             blueOutputPin.value()->getGlobalId().get(),
                                             textureSampleVariable );
    }

    Optional< const AlphaOutPin* > alphaOutputPin = getOutputPinByLocalId< AlphaOutPin >();
    if( alphaOutputPin && context.isPinConnected< AlphaOutPin >() ) {
        isAnyOutPinConnected = true;
        textureSampleCode += format::format( "float pin_{:x} = {}.w; // alpha \n",
                                             alphaOutputPin.value()->getGlobalId().get(),
                                             textureSampleVariable );
    }

    if( isAnyOutPinConnected ) {
        generator.appendCode( textureSampleCode );
    }
}

void SampleTextureNode::onChanged( assets::AssetSystem& assetSystem ) {
    if( m_texture.hasAssetId() ) {
        if( m_texture.isValid() == false ) {
            assetSystem.getAsset( m_texture.getId(), m_texture );
        }
    } else {
        m_texture.reset();
    }
}

#if ONYX_IS_EDITOR
StringView SampleTextureNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
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

node_graph::PinVisibility SampleTextureNode::doGetPinVisibility( StringId32 localPinId ) const {
    switch( localPinId ) {
    case TextureInPin::LocalId:
        return node_graph::PinVisibility::InNode;
    default:
        return node_graph::PinVisibility::Default;
    }
}
#endif

} // namespace onyx::graphics::shader_graph_nodes
