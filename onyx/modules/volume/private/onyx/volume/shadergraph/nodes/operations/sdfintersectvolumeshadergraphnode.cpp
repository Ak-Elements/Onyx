#include <onyx/volume/shadergraph/nodes/operations/sdfintersectvolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
void SdfIntersectVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void SdfIntersectVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                          rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    if( ( context.isPinConnected< OutPinGradient >() == false ) &&
        ( context.isPinConnected< OutPinIsoValue >() == false ) )
        return;

    generator.addImport( "includes.volume.csg.operations.intersect" );

    String intersectVariableName = format::format( "intersectNode_{:x}", getId().get() );

    String gradientOutVariableName = format::format( "pin_{:x}", m_gradientOutPin.getGlobalId().get() );
    String isoValueOutVariableName = format::format( "pin_{:x}", m_isoValueOutPin.getGlobalId().get() );

    generator.appendCode( format::format(
        "float4 {} = GetIntersection(float4({}, {}), float4({}, {}));\n",
        intersectVariableName,
        m_gradientSource0InPin.isConnected()
            ? format::format( "pin_{:x}", m_gradientSource0InPin.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPinGradientSource0 >() ),
        m_isoValueSource0InPin.isConnected()
            ? format::format( "pin_{:x}", m_isoValueSource0InPin.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPinIsoValueSource0 >() ),
        m_gradientSource1InPin.isConnected()
            ? format::format( "pin_{:x}", m_gradientSource1InPin.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPinGradientSource1 >() ),
        m_isoValueSource0InPin.isConnected()
            ? format::format( "pin_{:x}", m_isoValueSource1InPin.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPinIsoValueSource1 >() ) ) );
    generator.appendCode( format::format( "float {} = {}.w;\n", isoValueOutVariableName, intersectVariableName ) );
    generator.appendCode( format::format( "float3 {} = {}.xyz;\n", gradientOutVariableName, intersectVariableName ) );
}

node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::getInputPin( uint32_t index ) {
    switch( index ) {
    case 0:
        return &m_isoValueSource0InPin;
    case 1:
        return &m_gradientSource0InPin;
    case 2:
        return &m_isoValueSource1InPin;
    case 3:
        return &m_gradientSource1InPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

const node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::getInputPin( uint32_t index ) const {
    switch( index ) {
    case 0:
        return &m_isoValueSource0InPin;
    case 1:
        return &m_gradientSource0InPin;
    case 2:
        return &m_isoValueSource1InPin;
    case 3:
        return &m_gradientSource1InPin;
    }

    ONYX_ASSERT( false, "Invalid pin index" );
    return nullptr;
}

node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::getOutputPin( uint32_t index ) {
    ONYX_ASSERT( index < getOutputPinCount(), "Invalid pin index" );

    if( index == 0 )
        return &m_isoValueOutPin;

    return &m_gradientOutPin;
}

const node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::getOutputPin( uint32_t index ) const {
    ONYX_ASSERT( index < getOutputPinCount(), "Invalid pin index" );
    if( index == 0 )
        return &m_isoValueOutPin;

    return &m_gradientOutPin;
}

#if ONYX_IS_EDITOR
StringView SdfIntersectVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
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
