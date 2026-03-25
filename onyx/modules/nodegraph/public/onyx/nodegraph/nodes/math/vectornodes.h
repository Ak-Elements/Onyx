#pragma once

#include <onyx/geometry/vector.h>

#include <onyx/nodegraph/executioncontext.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in2out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in3out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in4out.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode3in1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode4in1out.h>
#include <onyx/nodegraph/nodes/math/vectornodes_editor.h>
#include <onyx/nodegraph/nodes/node.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::node_graph {
template < typename NodeType, typename InVectorT, typename OutVectorT, CompileTimeString TypeIdString >
class SwizzleVectorComponentsNode : public FixedPinNode_1_In_1_Out< NodeType, InVectorT, OutVectorT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_1_In_1_Out< NodeType, InVectorT, OutVectorT >;

    void OnUpdate( ExecutionContext& context ) const override {
        InVectorT inValue = context.GetPinData< typename Super::InPin >();
        OutVectorT& outValue = context.GetPinData< typename Super::OutPin >();

        if constexpr ( is_specialization_of_v< Vector2, OutVectorT > ) {
            outValue = OutVectorT( inValue[ OutIndexMapping[ 0 ] ], inValue[ OutIndexMapping[ 1 ] ] );
        } else if constexpr ( is_specialization_of_v< Vector3, OutVectorT > ) {
            outValue = OutVectorT( inValue[ OutIndexMapping[ 0 ] ],
                                   inValue[ OutIndexMapping[ 1 ] ],
                                   inValue[ OutIndexMapping[ 2 ] ] );
        } else if constexpr ( is_specialization_of_v< Vector4, OutVectorT > ) {
            outValue = OutVectorT( inValue[ OutIndexMapping[ 0 ] ],
                                   inValue[ OutIndexMapping[ 1 ] ],
                                   inValue[ OutIndexMapping[ 2 ] ],
                                   inValue[ OutIndexMapping[ 3 ] ] );
        }
    }

  protected:
    bool OnSerialize( Serializer& serializer ) const override {
        serializer.write< "swizzleMask" >( Mask );
        serializer.write< "swizzle" >( OutIndexMapping );
        return true;
    }

    bool OnDeserialize( const Deserializer& deserializer ) override {
        deserializer.read< "swizzleMask" >( Mask );
        deserializer.read< "swizzle" >( OutIndexMapping );
        return true;
    }

#if ONYX_IS_EDITOR
  protected:
    bool OnDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) override {
        bool hasModified = Super::OnDrawInPropertyGrid( constantPinData );

        // Optional<Vector4u8> swizzle = Editor::EditorSwizzleVectorControl<OutVectorT, InVectorT>::Draw(Mask);
        // if (swizzle.has_value())
        //{
        //     hasModified = true;
        //     OutIndexMapping = swizzle.value();
        // }

        return hasModified;
    }

  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin::LocalId:
            return "In";
        case Super::OutPin::LocalId:
            return enums::toString( Mask );
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif

  private:
    SwizzleMask Mask = is_specialization_of_v< Vector2, OutVectorT >   ? SwizzleMask::XY
                       : is_specialization_of_v< Vector3, OutVectorT > ? SwizzleMask::XYZ
                                                                       : SwizzleMask::XYW;

    Vector4u8 OutIndexMapping{ 0, 1, 2, 3 };
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class GetVector2Components : public FixedPinNode_1_In_2_Out< NodeType, Vector2< ScalarT >, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_1_In_2_Out< NodeType, Vector2< ScalarT >, ScalarT, ScalarT >;

    void OnUpdate( ExecutionContext& context ) const override {
        Vector2< ScalarT > inPin = context.GetPinData< typename Super::InPin >();

        ScalarT& out0 = context.GetPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.GetPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];
    }

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin::LocalId:
            return "In";
        case Super::OutPin0::LocalId:
            return "X";
        case Super::OutPin1::LocalId:
            return "Y";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class GetVector3Components : public FixedPinNode_1_In_3_Out< NodeType, Vector3< ScalarT >, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_1_In_3_Out< NodeType, Vector3< ScalarT >, ScalarT, ScalarT, ScalarT >;

    void OnUpdate( ExecutionContext& context ) const override {
        Vector3< ScalarT > inPin = context.GetPinData< typename Super::InPin >();

        ScalarT& out0 = context.GetPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.GetPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];

        ScalarT& out2 = context.GetPinData< typename Super::OutPin2 >();
        out2 = inPin[ 2 ];
    }

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin::LocalId:
            return "In";
        case Super::OutPin0::LocalId:
            return "X";
        case Super::OutPin1::LocalId:
            return "Y";
        case Super::OutPin2::LocalId:
            return "Z";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class GetVector4Components
    : public FixedPinNode_1_In_4_Out< NodeType, Vector4< ScalarT >, ScalarT, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_1_In_4_Out< NodeType, Vector4< ScalarT >, ScalarT, ScalarT, ScalarT, ScalarT >;

    void OnUpdate( ExecutionContext& context ) const override {
        Vector4< ScalarT > inPin = context.GetPinData< typename Super::InPin >();

        ScalarT& out0 = context.GetPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.GetPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];

        ScalarT& out2 = context.GetPinData< typename Super::OutPin2 >();
        out2 = inPin[ 2 ];

        ScalarT& out3 = context.GetPinData< typename Super::OutPin2 >();
        out3 = inPin[ 3 ];
    }

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin::LocalId:
            return "In";
        case Super::OutPin0::LocalId:
            return "X";
        case Super::OutPin1::LocalId:
            return "Y";
        case Super::OutPin2::LocalId:
            return "Z";
        case Super::OutPin3::LocalId:
            return "W";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class CreateVector2 : public FixedPinNode_2_In_1_Out< NodeType, ScalarT, ScalarT, Vector2< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_2_In_1_Out< NodeType, ScalarT, ScalarT, Vector2< ScalarT > >;

    void OnUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.GetPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.GetPinData< typename Super::InPin1 >();

        Vector2< ScalarT >& out = context.GetPinData< typename Super::OutPin >();
        out = Vector2( inPin0, inPin1 );
    }

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin0::LocalId:
            return "X";
        case Super::InPin1::LocalId:
            return "Y";
        case Super::OutPin::LocalId:
            return "Vector2";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class CreateVector3 : public FixedPinNode3In1Out< NodeType, ScalarT, ScalarT, ScalarT, Vector3< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode3In1Out< NodeType, ScalarT, ScalarT, ScalarT, Vector3< ScalarT > >;

    void OnUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.GetPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.GetPinData< typename Super::InPin1 >();
        ScalarT inPin2 = context.GetPinData< typename Super::InPin2 >();

        Vector3< ScalarT >& out = context.GetPinData< typename Super::OutPin >();
        out = Vector3( inPin0, inPin1, inPin2 );
    }

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin0::LocalId:
            return "X";
        case Super::InPin1::LocalId:
            return "Y";
        case Super::InPin2::LocalId:
            return "Z";
        case Super::OutPin::LocalId:
            return "Vector3";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class CreateVector4
    : public FixedPinNode_4_In_1_Out< NodeType, ScalarT, ScalarT, ScalarT, ScalarT, Vector4< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    StringId32 GetTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode_4_In_1_Out< NodeType, ScalarT, ScalarT, ScalarT, ScalarT, Vector4< ScalarT > >;

    void OnUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.GetPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.GetPinData< typename Super::InPin1 >();
        ScalarT inPin2 = context.GetPinData< typename Super::InPin2 >();
        ScalarT inPin3 = context.GetPinData< typename Super::InPin3 >();

        Vector4< ScalarT >& out = context.GetPinData< typename Super::OutPin >();
        out = Vector4( inPin0, inPin1, inPin2, inPin3 );
    }

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch ( pinId ) {
        case Super::InPin0::LocalId:
            return "X";
        case Super::InPin1::LocalId:
            return "Y";
        case Super::InPin2::LocalId:
            return "Z";
        case Super::InPin3::LocalId:
            return "W";
        case Super::OutPin::LocalId:
            return "Vector4";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};

namespace Nodes {
using GetVector2f32Components = GetVector2Components< Node,
                                                      float32,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector2f32Components" >;
using GetVector2f64Components = GetVector2Components< Node,
                                                      float64,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector2f64Components" >;
using GetVector2s32Components = GetVector2Components< Node,
                                                      int32_t,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector2s32Components" >;
using GetVector2s64Components = GetVector2Components< Node,
                                                      int64_t,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector2s64Components" >;

using CreateVector2f32 = CreateVector2< Node, float32, "Nodeonyx::NodeGraph::Nodes::CreateVector2f32" >;
using CreateVector2f64 = CreateVector2< Node, float64, "Nodeonyx::NodeGraph::Nodes::CreateVector2f64" >;
using CreateVector2s32 = CreateVector2< Node, int32_t, "Nodeonyx::NodeGraph::Nodes::CreateVector2s32" >;
using CreateVector2s64 = CreateVector2< Node, int64_t, "Nodeonyx::NodeGraph::Nodes::CreateVector2s64" >;

using Swizzle2DVector2f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f32,
                                                         Vector2f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector2f32" >;
using Swizzle2DVector3f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f32,
                                                         Vector2f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector3f32" >;
using Swizzle2DVector4f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f32,
                                                         Vector2f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector4f32" >;

using Swizzle2DVector2f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f64,
                                                         Vector2f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector2f64" >;
using Swizzle2DVector3f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f64,
                                                         Vector2f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector3f64" >;
using Swizzle2DVector4f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f64,
                                                         Vector2f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector4f64" >;

using Swizzle2DVector2s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s32,
                                                         Vector2s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector2s32" >;
using Swizzle2DVector3s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s32,
                                                         Vector2s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector3s32" >;
using XSwizzle2DVector4s32 = SwizzleVectorComponentsNode< Node,
                                                          Vector4s32,
                                                          Vector2s32,
                                                          "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector4s32" >;

using Swizzle2DVector2s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s64,
                                                         Vector2s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector2s64" >;
using Swizzle2DVector3s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s64,
                                                         Vector2s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector3s64" >;
using Swizzle2DVector4s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s64,
                                                         Vector2s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle2DVector4s64" >;

// Vector 3
using GetVector3f32Components = GetVector3Components< Node,
                                                      float32,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector3f32Components" >;
using GetVector3f64Components = GetVector3Components< Node,
                                                      float64,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector3f64Components" >;
using GetVector3s32Components = GetVector3Components< Node,
                                                      int32_t,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector3s32Components" >;
using GetVector3s64Components = GetVector3Components< Node,
                                                      int64_t,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector3s64Components" >;

using CreateVector3f32 = CreateVector3< Node, float32, "Nodeonyx::NodeGraph::Nodes::CreateVector3f32" >;
using CreateVector3f64 = CreateVector3< Node, float64, "Nodeonyx::NodeGraph::Nodes::CreateVector3f64" >;
using CreateVector3s32 = CreateVector3< Node, int32_t, "Nodeonyx::NodeGraph::Nodes::CreateVector3s32" >;
using CreateVector3s64 = CreateVector3< Node, int64_t, "Nodeonyx::NodeGraph::Nodes::CreateVector3s64" >;

using Swizzle3DVector2f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f32,
                                                         Vector3f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector2f32" >;
using Swizzle3DVector3f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f32,
                                                         Vector3f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector3f32" >;
using Swizzle3DVector4f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f32,
                                                         Vector3f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector4f32" >;

using Swizzle3DVector2f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f64,
                                                         Vector3f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector2f64" >;
using Swizzle3DVector3f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f64,
                                                         Vector3f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector3f64" >;
using Swizzle3DVector4f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f64,
                                                         Vector3f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector4f64" >;

using Swizzle3DVector2s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s32,
                                                         Vector3s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector2s32" >;
using Swizzle3DVector3s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s32,
                                                         Vector3s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector3s32" >;
using Swizzle3DVector4s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s32,
                                                         Vector3s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector4s32" >;

using Swizzle3DVector2s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s64,
                                                         Vector3s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector2s64" >;
using Swizzle3DVector3s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s64,
                                                         Vector3s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector3s64" >;
using Swizzle3DVector4s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s64,
                                                         Vector3s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle3DVector4s64" >;

// Vector 4
using GetVector4f32Components = GetVector4Components< Node,
                                                      float32,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector4f32Components" >;
using GetVector4f64Components = GetVector4Components< Node,
                                                      float64,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector4f64Components" >;
using GetVector4s32Components = GetVector4Components< Node,
                                                      int32_t,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector4s32Components" >;
using GetVector4s64Components = GetVector4Components< Node,
                                                      int64_t,
                                                      "Nodeonyx::NodeGraph::Nodes::GetVector4s64Components" >;

using CreateVector4f32 = CreateVector4< Node, float32, "Nodeonyx::NodeGraph::Nodes::CreateVector4f32" >;
using CreateVector4f64 = CreateVector4< Node, float64, "Nodeonyx::NodeGraph::Nodes::CreateVector4f64" >;
using CreateVector4s32 = CreateVector4< Node, int32_t, "Nodeonyx::NodeGraph::Nodes::CreateVector4s32" >;
using CreateVector4s64 = CreateVector4< Node, int64_t, "Nodeonyx::NodeGraph::Nodes::CreateVector4s64" >;

using Swizzle4DVector2f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f32,
                                                         Vector4f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector2f32" >;
using Swizzle4DVector3f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f32,
                                                         Vector4f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector3f32" >;
using Swizzle4DVector4f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f32,
                                                         Vector4f32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector4f32" >;

using Swizzle4DVector2f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f64,
                                                         Vector4f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector2f64" >;
using Swizzle4DVector3f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f64,
                                                         Vector4f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector3f64" >;
using Swizzle4DVector4f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f64,
                                                         Vector4f64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector4f64" >;

using Swizzle4DVector2s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s32,
                                                         Vector4s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector2s32" >;
using Swizzle4DVector3s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s32,
                                                         Vector4s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector3s32" >;
using Swizzle4DVector4s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s32,
                                                         Vector4s32,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector4s32" >;

using Swizzle4DVector2s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s64,
                                                         Vector4s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector2s64" >;
using Swizzle4DVector3s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s64,
                                                         Vector4s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector3s64" >;
using Swizzle4DVector4s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s64,
                                                         Vector4s64,
                                                         "Nodeonyx::NodeGraph::Nodes::Swizzle4DVector4s64" >;
} // namespace Nodes

} // namespace onyx::node_graph
