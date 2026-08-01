#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
void FragmentShaderOutNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                              rhi::ShaderGenerator& generator ) const {
    if( getInputPin().isConnected() ) {
        generator.appendCode( format::format( "outColor = pin_{:x};", getInputPin().getLinkedPinGlobalId().get() ) );
    } else {
        const Vector4f32& outColor = context.getPinData< InPin >();
        generator.appendCode( format::format( "outColor = vec4({}, {}, {}, {});",
                                              outColor[ 0 ],
                                              outColor[ 1 ],
                                              outColor[ 2 ],
                                              outColor[ 3 ] ) );
    }
}

#if ONYX_IS_EDITOR
StringView FragmentShaderOutNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin::LocalId:
        return "Color";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif

void PBRMaterialShaderOutNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                 rhi::ShaderGenerator& generator ) const {
    generator.appendCode( "PBRMaterial material;\n" );

    Optional< const AlbedoInPin* > albedoPin = getInputPinByLocalId< AlbedoInPin >();
    if( albedoPin ) {
        const AlbedoInPin& albedo = *albedoPin.value();
        generator.appendCode( format::format(
            "material.Albedo = {};\n",
            albedo.isConnected() ? format::format( "pin_{:x}", albedo.getLinkedPinGlobalId().get() )
                                 : rhi::ShaderGenerator::generateShaderValue( context.getPinData< AlbedoInPin >() ) ) );
    }

    Optional< const NormalInPin* > normalPin = getInputPinByLocalId< NormalInPin >();
    if( normalPin ) {
        const NormalInPin& normal = *normalPin.value();
        generator.appendCode( format::format(
            "material.Normal = {};\n",
            normal.isConnected() ? format::format( "pin_{:x}", normal.getLinkedPinGlobalId().get() )
                                 : rhi::ShaderGenerator::generateShaderValue( context.getPinData< NormalInPin >() ) ) );
    }

    Optional< const MetalnessInPin* > metallicPin = getInputPinByLocalId< MetalnessInPin >();
    if( metallicPin ) {
        const MetalnessInPin& metalness = *metallicPin.value();
        generator.appendCode( format::format(
            "material.Metalness = {};\n",
            metalness.isConnected()
                ? format::format( "pin_{:x}", metalness.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< MetalnessInPin >() ) ) );
    }

    Optional< const RoughnessInPin* > roughnessPin = getInputPinByLocalId< RoughnessInPin >();
    if( metallicPin ) {
        const RoughnessInPin& roughness = *roughnessPin.value();
        generator.appendCode( format::format(
            "material.Roughness = {};\n",
            roughness.isConnected()
                ? format::format( "pin_{:x}", roughness.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< RoughnessInPin >() ) ) );
    }
}

#if ONYX_IS_EDITOR
std::any PBRMaterialShaderOutNode::createDefaultForPin( StringId32 pinId ) const {
    if( pinId == RoughnessInPin::LocalId ) {
        return 0.5f;
    }

    return Super::createDefaultForPin( pinId );
}
#endif

node_graph::PinBase* PBRMaterialShaderOutNode::getInputPin( uint32_t index ) {
    switch( index ) {
    case 0:
        return &m_albedoInPin;
    case 1:
        return &m_normalInPin;
    case 2:
        return &m_metalnessInPin;
    case 3:
        return &m_roughnessInPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

const node_graph::PinBase* PBRMaterialShaderOutNode::getInputPin( uint32_t index ) const {
    switch( index ) {
    case 0:
        return &m_albedoInPin;
    case 1:
        return &m_normalInPin;
    case 2:
        return &m_metalnessInPin;
    case 3:
        return &m_roughnessInPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

node_graph::PinBase* PBRMaterialShaderOutNode::getOutputPin( uint32_t /*index*/ ) {
    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

const node_graph::PinBase* PBRMaterialShaderOutNode::getOutputPin( uint32_t /*index*/ ) const {
    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

} // namespace onyx::graphics::shader_graph_nodes
