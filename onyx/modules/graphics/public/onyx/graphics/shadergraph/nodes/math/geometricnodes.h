#pragma once
#include <onyx/nodegraph/nodes/math/geometricnodes.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::NodeGraph {
template < template < typename ScalarT > class VectorT, typename ScalarT, CompileTimeString TypeIdString >
class ScaleVectorNode< graphics::ShaderGraphNode, VectorT, ScalarT, TypeIdString >
    : public FixedPinNode_2_In_1_Out< graphics::ShaderGraphNode, VectorT< ScalarT >, ScalarT, VectorT< ScalarT > > {
  public:
    static constexpr bool HasAliases = true;

    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_2_In_1_Out< graphics::ShaderGraphNode, VectorT< ScalarT >, ScalarT, VectorT< ScalarT > >;

    void OnUpdate( ExecutionContext& context ) const override {
        VectorT< ScalarT > pin0 = context.GetPinData< typename Super::InPin0 >();
        ScalarT pin1 = context.GetPinData< typename Super::InPin1 >();

        VectorT< ScalarT >& out = context.GetPinData< typename Super::OutPin >();
        out = pin0 * pin1;
    }

#if ONYX_IS_EDITOR
  private:
    void DoGenerateShader( const ExecutionContext& context, graphics::ShaderGenerator& generator ) const override {
        generator.AppendCode( "// Scale vector node \n" );
        generator.AppendCode( format::Format(
            "{} pin_{:x} = ",
            graphics::ShaderGenerator::GetTypeAsShaderTypeString< typename Super::OutPin::DataType >().c_str(),
            Super::GetOutputPin().GetGlobalId().Get() ) );

        const typename Super::InPin0& inputPin0 = Super::GetInputPin0();
        generator.AppendCode(
            inputPin0.IsConnected()
                ? format::Format( "pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get() )
                : graphics::ShaderGenerator::GenerateShaderValue( context.GetPinData< typename Super::InPin0 >() ) );
        generator.AppendCode( " * " );
        const typename Super::InPin1& inputPin1 = Super::GetInputPin1();
        generator.AppendCode(
            inputPin1.IsConnected()
                ? format::Format( "pin_{:x}", inputPin1.GetLinkedPinGlobalId().Get() )
                : graphics::ShaderGenerator::GenerateShaderValue( context.GetPinData< typename Super::InPin1 >() ) );
        generator.AppendCode( "; \n" );
    }

  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin0::LocalId:
            return "Vector";
        case Super::InPin1::LocalId:
            return "Scale";
        case Super::OutPin::LocalId:
            return "Result";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};
} // namespace onyx::NodeGraph

namespace onyx::graphics::shader_graph_nodes {
using ScaleNodeVector2f32 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector2, float32, "onyx::graphics::ShaderGraph::ScaleVector2f32" >;
using ScaleNodeVector2f64 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector2, float64, "onyx::graphics::ShaderGraph::ScaleVector2f64" >;
using ScaleNodeVector2s32 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector2, int32_t, "onyx::graphics::ShaderGraph::ScaleVector2s32" >;
using ScaleNodeVector2s64 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector2, int64_t, "onyx::graphics::ShaderGraph::ScaleVector2s64" >;

using ScaleNodeVector3f32 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector3, float32, "onyx::graphics::ShaderGraph::ScaleVector3f32" >;
using ScaleNodeVector3f64 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector3, float64, "onyx::graphics::ShaderGraph::ScaleVector3f64" >;
using ScaleNodeVector3s32 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector3, int32_t, "onyx::graphics::ShaderGraph::ScaleVector3s32" >;
using ScaleNodeVector3s64 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector3, int64_t, "onyx::graphics::ShaderGraph::ScaleVector3s64" >;

using ScaleNodeVector4f32 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector4, float32, "onyx::graphics::ShaderGraph::ScaleVector4f32" >;
using ScaleNodeVector4f64 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector4, float64, "onyx::graphics::ShaderGraph::ScaleVector4f64" >;
using ScaleNodeVector4s32 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector4, int32_t, "onyx::graphics::ShaderGraph::ScaleVector4s32" >;
using ScaleNodeVector4s64 = NodeGraph::
    ScaleVectorNode< ShaderGraphNode, Vector4, int64_t, "onyx::graphics::ShaderGraph::ScaleVector4s64" >;
} // namespace onyx::graphics::shader_graph_nodes
