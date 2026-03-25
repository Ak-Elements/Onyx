#include <onyx/graphics/shadergraph/nodes/fragmentshaderoutnode.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
void FragmentShaderOutNode::DoGenerateShader( const node_graph::ExecutionContext& context,
                                              rhi::ShaderGenerator& generator ) const {
    if ( GetInputPin().IsConnected() ) {
        generator.AppendCode( format::format( "outColor = pin_{:x};", GetInputPin().GetLinkedPinGlobalId().get() ) );
    } else {
        const Vector4f32& outColor = context.GetPinData< InPin >();
        generator.AppendCode( format::format( "outColor = vec4({}, {}, {}, {});",
                                              outColor[ 0 ],
                                              outColor[ 1 ],
                                              outColor[ 2 ],
                                              outColor[ 3 ] ) );
    }
}

#if ONYX_IS_EDITOR
StringView FragmentShaderOutNode::GetPinName( StringId32 pinId ) const {
    switch ( pinId ) {
    case InPin::LocalId:
        return "Color";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif

void PBRMaterialShaderOutNode::DoGenerateShader( const node_graph::ExecutionContext& context,
                                                 rhi::ShaderGenerator& generator ) const {
    generator.AppendCode( "PBRMaterial material;\n" );

    Optional< const AlbedoInPin* > albedoPin = GetInputPinByLocalId< AlbedoInPin >();
    if ( albedoPin ) {
        const AlbedoInPin& albedo = *albedoPin.value();
        generator.AppendCode( format::format(
            "material.Albedo = {};\n",
            albedo.IsConnected() ? format::format( "pin_{:x}", albedo.GetLinkedPinGlobalId().get() )
                                 : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< AlbedoInPin >() ) ) );
    }

    Optional< const NormalInPin* > normalPin = GetInputPinByLocalId< NormalInPin >();
    if ( normalPin ) {
        const NormalInPin& normal = *normalPin.value();
        generator.AppendCode( format::format(
            "material.Normal = {};\n",
            normal.IsConnected() ? format::format( "pin_{:x}", normal.GetLinkedPinGlobalId().get() )
                                 : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< NormalInPin >() ) ) );
    }

    Optional< const MetalnessInPin* > metallicPin = GetInputPinByLocalId< MetalnessInPin >();
    if ( metallicPin ) {
        const MetalnessInPin& metalness = *metallicPin.value();
        generator.AppendCode( format::format(
            "material.Metalness = {};\n",
            metalness.IsConnected()
                ? format::format( "pin_{:x}", metalness.GetLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< MetalnessInPin >() ) ) );
    }

    Optional< const RoughnessInPin* > roughnessPin = GetInputPinByLocalId< RoughnessInPin >();
    if ( metallicPin ) {
        const RoughnessInPin& roughness = *roughnessPin.value();
        generator.AppendCode( format::format(
            "material.Roughness = {};\n",
            roughness.IsConnected()
                ? format::format( "pin_{:x}", roughness.GetLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< RoughnessInPin >() ) ) );
    }
}

#if ONYX_IS_EDITOR
std::any PBRMaterialShaderOutNode::CreateDefaultForPin( StringId32 pinId ) const {
    if ( pinId == RoughnessInPin::LocalId ) {
        return 0.5f;
    }

    return Super::CreateDefaultForPin( pinId );
}
#endif

node_graph::PinBase* PBRMaterialShaderOutNode::GetInputPin( uint32_t index ) {
    switch ( index ) {
    case 0:
        return &m_AlbedoInPin;
    case 1:
        return &m_NormalInPin;
    case 2:
        return &m_MetalnessInPin;
    case 3:
        return &m_RoughnessInPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

const node_graph::PinBase* PBRMaterialShaderOutNode::GetInputPin( uint32_t index ) const {
    switch ( index ) {
    case 0:
        return &m_AlbedoInPin;
    case 1:
        return &m_NormalInPin;
    case 2:
        return &m_MetalnessInPin;
    case 3:
        return &m_RoughnessInPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

node_graph::PinBase* PBRMaterialShaderOutNode::GetOutputPin( uint32_t /*index*/ ) {
    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

const node_graph::PinBase* PBRMaterialShaderOutNode::GetOutputPin( uint32_t /*index*/ ) const {
    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

} // namespace onyx::graphics::shader_graph_nodes
