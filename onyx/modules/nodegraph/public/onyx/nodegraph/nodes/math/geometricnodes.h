#pragma once

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::node_graph {
template < typename NodeType,
           template < typename ScalarT > class VectorT,
           typename ScalarT,
           CompileTimeString TypeIdString >
class ScaleVectorNode : public FixedPinNode2In1Out< NodeType, VectorT< ScalarT >, ScalarT, VectorT< ScalarT > > {
  public:
    static constexpr bool HasAliases = true;

    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< NodeType, VectorT< ScalarT >, ScalarT, VectorT< ScalarT > >;

    void onUpdate( ExecutionContext& context ) const override {
        VectorT< ScalarT > pin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT pin1 = context.getPinData< typename Super::InPin1 >();

        VectorT< ScalarT >& out = context.getPinData< typename Super::OutPin >();
        out = pin0 * pin1;
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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

namespace nodes {
using ScaleNodeVector2f32 = ScaleVectorNode< Node, Vector2, float32, "onyx::node_graph::nodes::ScaleVector2f32" >;
using ScaleNodeVector2f64 = ScaleVectorNode< Node, Vector2, float64, "onyx::node_graph::nodes::ScaleVector2f64" >;
using ScaleNodeVector2s32 = ScaleVectorNode< Node, Vector2, int32_t, "onyx::node_graph::nodes::ScaleVector2s32" >;
using ScaleNodeVector2s64 = ScaleVectorNode< Node, Vector2, int64_t, "onyx::node_graph::nodes::ScaleVector2s64" >;

using ScaleNodeVector3f32 = ScaleVectorNode< Node, Vector3, float32, "onyx::node_graph::nodes::ScaleVector3f32" >;
using ScaleNodeVector3f64 = ScaleVectorNode< Node, Vector3, float64, "onyx::node_graph::nodes::ScaleVector3f64" >;
using ScaleNodeVector3s32 = ScaleVectorNode< Node, Vector3, int32_t, "onyx::node_graph::nodes::ScaleVector3s32" >;
using ScaleNodeVector3s64 = ScaleVectorNode< Node, Vector3, int64_t, "onyx::node_graph::nodes::ScaleVector3s64" >;

using ScaleNodeVector4f32 = ScaleVectorNode< Node, Vector4, float32, "onyx::node_graph::nodes::ScaleVector4f32" >;
using ScaleNodeVector4f64 = ScaleVectorNode< Node, Vector4, float64, "onyx::node_graph::nodes::ScaleVector4f64" >;
using ScaleNodeVector4s32 = ScaleVectorNode< Node, Vector4, int32_t, "onyx::node_graph::nodes::ScaleVector4s32" >;
using ScaleNodeVector4s64 = ScaleVectorNode< Node, Vector4, int64_t, "onyx::node_graph::nodes::ScaleVector4s64" >;
} // namespace nodes
} // namespace onyx::node_graph
