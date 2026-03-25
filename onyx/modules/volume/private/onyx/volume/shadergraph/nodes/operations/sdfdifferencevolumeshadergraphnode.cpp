#include <onyx/volume/shadergraph/nodes/operations/sdfdifferencevolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
void SdfDifferenceVolumeShaderGraphNode::OnUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void SdfDifferenceVolumeShaderGraphNode::DoGenerateShader( const node_graph::ExecutionContext& context,
                                                           rhi::ShaderGenerator& generator ) const {
    if ( generator.GetStage() != rhi::ShaderStage::Fragment )
        return;

    if ( ( context.IsPinConnected< OutPinGradient >() == false ) &&
         ( context.IsPinConnected< OutPinIsoValue >() == false ) )
        return;

    generator.AddInclude( rhi::ShaderStage::All, "includes/volume/csg/operations/difference.h" );

    String differenceVariableName = format::format( "differenceNode_{:x}", GetId().get() );

    String isoValueOutVariableName = format::format( "pin_{:x}", m_IsoValueOutPin.GetGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", m_GradientOutPin.GetGlobalId().get() );

    generator.AppendCode( format::format(
        "vec4 {} = GetDifference(vec4({}, {}), vec4({}, {}));\n",
        differenceVariableName,
        m_GradientSource0InPin.IsConnected()
            ? format::format( "pin_{:x}", m_GradientSource0InPin.GetLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< InPinGradientSource0 >() ),
        m_IsoValueSource0InPin.IsConnected()
            ? format::format( "pin_{:x}", m_IsoValueSource0InPin.GetLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< InPinIsoValueSource0 >() ),
        m_GradientSource1InPin.IsConnected()
            ? format::format( "pin_{:x}", m_GradientSource1InPin.GetLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< InPinGradientSource1 >() ),
        m_IsoValueSource0InPin.IsConnected()
            ? format::format( "pin_{:x}", m_IsoValueSource1InPin.GetLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< InPinIsoValueSource1 >() ) ) );
    generator.AppendCode( format::format( "float {} = {}.w;\n", isoValueOutVariableName, differenceVariableName ) );
    generator.AppendCode( format::format( "vec3 {} = {}.xyz;\n", gradientOutVariableName, differenceVariableName ) );
}

node_graph::PinBase* SdfDifferenceVolumeShaderGraphNode::GetInputPin( uint32_t index ) {
    switch ( index ) {
    case 0:
        return &m_IsoValueSource0InPin;
    case 1:
        return &m_GradientSource0InPin;
    case 2:
        return &m_IsoValueSource1InPin;
    case 3:
        return &m_GradientSource1InPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

const node_graph::PinBase* SdfDifferenceVolumeShaderGraphNode::GetInputPin( uint32_t index ) const {
    switch ( index ) {
    case 0:
        return &m_IsoValueSource0InPin;
    case 1:
        return &m_GradientSource0InPin;
    case 2:
        return &m_IsoValueSource1InPin;
    case 3:
        return &m_GradientSource1InPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

node_graph::PinBase* SdfDifferenceVolumeShaderGraphNode::GetOutputPin( uint32_t index ) {
    ONYX_ASSERT( index < GetOutputPinCount(), "Invalid pin index" );

    if ( index == 0 )
        return &m_IsoValueOutPin;

    return &m_GradientOutPin;
}

const node_graph::PinBase* SdfDifferenceVolumeShaderGraphNode::GetOutputPin( uint32_t index ) const {
    ONYX_ASSERT( index < GetOutputPinCount(), "Invalid pin index" );
    if ( index == 0 )
        return &m_IsoValueOutPin;

    return &m_GradientOutPin;
}

#if ONYX_IS_EDITOR
StringView SdfDifferenceVolumeShaderGraphNode::GetPinName( StringId32 pinId ) const {
    switch ( pinId ) {
    case InPinIsoValueSource0::LocalId:
        return "Iso Value A";
    case InPinGradientSource0::LocalId:
        return "Gradient A";
    case InPinIsoValueSource1::LocalId:
        return "Iso Value B";
    case InPinGradientSource1::LocalId:
        return "Gradient B";

    case OutPinIsoValue::LocalId:
        return "Iso Value";
    case OutPinGradient::LocalId:
        return "Gradient";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
