#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/volume/source/sdfsample.h>

namespace onyx::volume {
void VolumeShaderGraphOutNode::onUpdate( node_graph::ExecutionContext& context ) const {
    SdfSample& outSample = context.get< SdfSample >();
    outSample = context.getPinData< InPin >();
}

void VolumeShaderGraphOutNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                 rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    const InPin& inputPin = getInputPin();
    if( inputPin.isConnected() ) {
        generator.appendCode( format::format( "return pin_{:x};", inputPin.getLinkedPinGlobalId().get() ) );
    } else {
        // TODO implement static return
    }
}

#if ONYX_IS_EDITOR
StringView VolumeShaderGraphOutNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
