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
class SwizzleVectorComponentsNode : public FixedPinNode1In1Out< NodeType, InVectorT, OutVectorT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In1Out< NodeType, InVectorT, OutVectorT >;

    void onUpdate( ExecutionContext& context ) const override {
        InVectorT inValue = context.getPinData< typename Super::InPin >();
        OutVectorT& outValue = context.getPinData< typename Super::OutPin >();

        if constexpr( is_specialization_of_v< Vector2, OutVectorT > ) {
            outValue = OutVectorT( inValue[ m_outIndexMapping[ 0 ] ], inValue[ m_outIndexMapping[ 1 ] ] );
        } else if constexpr( is_specialization_of_v< Vector3, OutVectorT > ) {
            outValue = OutVectorT( inValue[ m_outIndexMapping[ 0 ] ],
                                   inValue[ m_outIndexMapping[ 1 ] ],
                                   inValue[ m_outIndexMapping[ 2 ] ] );
        } else if constexpr( is_specialization_of_v< Vector4, OutVectorT > ) {
            outValue = OutVectorT( inValue[ m_outIndexMapping[ 0 ] ],
                                   inValue[ m_outIndexMapping[ 1 ] ],
                                   inValue[ m_outIndexMapping[ 2 ] ],
                                   inValue[ m_outIndexMapping[ 3 ] ] );
        }
    }

  protected:
    bool onSerialize( Serializer& serializer ) const override {
        serializer.write< "swizzleMask" >( m_mask );
        serializer.write< "swizzle" >( m_outIndexMapping );
        return true;
    }

    bool onDeserialize( const Deserializer& deserializer ) override {
        deserializer.read< "swizzleMask" >( m_mask );
        deserializer.read< "swizzle" >( m_outIndexMapping );
        return true;
    }

#if ONYX_IS_EDITOR
  protected:
    bool onDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) override {
        bool hasModified = Super::onDrawInPropertyGrid( constantPinData );

        // Optional<Vector4u8> swizzle = Editor::EditorSwizzleVectorControl<OutVectorT, InVectorT>::Draw(Mask);
        // if (swizzle.has_value())
        //{
        //     hasModified = true;
        //     OutIndexMapping = swizzle.value();
        // }

        return hasModified;
    }

  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case Super::InPin::LocalId:
            return "In";
        case Super::OutPin::LocalId:
            return enums::toString( m_mask );
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif

  private:
    SwizzleMask m_mask = is_specialization_of_v< Vector2, OutVectorT >   ? SwizzleMask::XY
                         : is_specialization_of_v< Vector3, OutVectorT > ? SwizzleMask::XYZ
                                                                         : SwizzleMask::XYW;

    Vector4u8 m_outIndexMapping{ 0, 1, 2, 3 };
};

template < typename NodeType, typename ScalarT, CompileTimeString TypeIdString >
class GetVector2Components : public FixedPinNode1In2Out< NodeType, Vector2< ScalarT >, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In2Out< NodeType, Vector2< ScalarT >, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        Vector2< ScalarT > inPin = context.getPinData< typename Super::InPin >();

        ScalarT& out0 = context.getPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.getPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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
class GetVector3Components : public FixedPinNode1In3Out< NodeType, Vector3< ScalarT >, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In3Out< NodeType, Vector3< ScalarT >, ScalarT, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        Vector3< ScalarT > inPin = context.getPinData< typename Super::InPin >();

        ScalarT& out0 = context.getPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.getPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];

        ScalarT& out2 = context.getPinData< typename Super::OutPin2 >();
        out2 = inPin[ 2 ];
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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
    : public FixedPinNode1In4Out< NodeType, Vector4< ScalarT >, ScalarT, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In4Out< NodeType, Vector4< ScalarT >, ScalarT, ScalarT, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        Vector4< ScalarT > inPin = context.getPinData< typename Super::InPin >();

        ScalarT& out0 = context.getPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.getPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];

        ScalarT& out2 = context.getPinData< typename Super::OutPin2 >();
        out2 = inPin[ 2 ];

        ScalarT& out3 = context.getPinData< typename Super::OutPin2 >();
        out3 = inPin[ 3 ];
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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
class CreateVector2 : public FixedPinNode2In1Out< NodeType, ScalarT, ScalarT, Vector2< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< NodeType, ScalarT, ScalarT, Vector2< ScalarT > >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.getPinData< typename Super::InPin1 >();

        Vector2< ScalarT >& out = context.getPinData< typename Super::OutPin >();
        out = Vector2( inPin0, inPin1 );
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode3In1Out< NodeType, ScalarT, ScalarT, ScalarT, Vector3< ScalarT > >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.getPinData< typename Super::InPin1 >();
        ScalarT inPin2 = context.getPinData< typename Super::InPin2 >();

        Vector3< ScalarT >& out = context.getPinData< typename Super::OutPin >();
        out = Vector3( inPin0, inPin1, inPin2 );
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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
class CreateVector4 : public FixedPinNode4In1Out< NodeType, ScalarT, ScalarT, ScalarT, ScalarT, Vector4< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode4In1Out< NodeType, ScalarT, ScalarT, ScalarT, ScalarT, Vector4< ScalarT > >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.getPinData< typename Super::InPin1 >();
        ScalarT inPin2 = context.getPinData< typename Super::InPin2 >();
        ScalarT inPin3 = context.getPinData< typename Super::InPin3 >();

        Vector4< ScalarT >& out = context.getPinData< typename Super::OutPin >();
        out = Vector4( inPin0, inPin1, inPin2, inPin3 );
    }

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override {
        switch( pinId ) {
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

namespace nodes {
using GetVector2f32Components = GetVector2Components< Node,
                                                      float32,
                                                      "onyx::node_graph::nodes::GetVector2f32Components" >;
using GetVector2f64Components = GetVector2Components< Node,
                                                      float64,
                                                      "onyx::node_graph::nodes::GetVector2f64Components" >;
using GetVector2s32Components = GetVector2Components< Node,
                                                      int32_t,
                                                      "onyx::node_graph::nodes::GetVector2s32Components" >;
using GetVector2s64Components = GetVector2Components< Node,
                                                      int64_t,
                                                      "onyx::node_graph::nodes::GetVector2s64Components" >;

using CreateVector2f32 = CreateVector2< Node, float32, "onyx::node_graph::nodes::CreateVector2f32" >;
using CreateVector2f64 = CreateVector2< Node, float64, "onyx::node_graph::nodes::CreateVector2f64" >;
using CreateVector2s32 = CreateVector2< Node, int32_t, "onyx::node_graph::nodes::CreateVector2s32" >;
using CreateVector2s64 = CreateVector2< Node, int64_t, "onyx::node_graph::nodes::CreateVector2s64" >;

using Swizzle2DVector2f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f32,
                                                         Vector2f32,
                                                         "onyx::node_graph::nodes::Swizzle2DVector2f32" >;
using Swizzle2DVector3f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f32,
                                                         Vector2f32,
                                                         "onyx::node_graph::nodes::Swizzle2DVector3f32" >;
using Swizzle2DVector4f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f32,
                                                         Vector2f32,
                                                         "onyx::node_graph::nodes::Swizzle2DVector4f32" >;

using Swizzle2DVector2f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f64,
                                                         Vector2f64,
                                                         "onyx::node_graph::nodes::Swizzle2DVector2f64" >;
using Swizzle2DVector3f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f64,
                                                         Vector2f64,
                                                         "onyx::node_graph::nodes::Swizzle2DVector3f64" >;
using Swizzle2DVector4f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f64,
                                                         Vector2f64,
                                                         "onyx::node_graph::nodes::Swizzle2DVector4f64" >;

using Swizzle2DVector2s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s32,
                                                         Vector2s32,
                                                         "onyx::node_graph::nodes::Swizzle2DVector2s32" >;
using Swizzle2DVector3s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s32,
                                                         Vector2s32,
                                                         "onyx::node_graph::nodes::Swizzle2DVector3s32" >;
using XSwizzle2DVector4s32 = SwizzleVectorComponentsNode< Node,
                                                          Vector4s32,
                                                          Vector2s32,
                                                          "onyx::node_graph::nodes::Swizzle2DVector4s32" >;

using Swizzle2DVector2s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s64,
                                                         Vector2s64,
                                                         "onyx::node_graph::nodes::Swizzle2DVector2s64" >;
using Swizzle2DVector3s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s64,
                                                         Vector2s64,
                                                         "onyx::node_graph::nodes::Swizzle2DVector3s64" >;
using Swizzle2DVector4s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s64,
                                                         Vector2s64,
                                                         "onyx::node_graph::nodes::Swizzle2DVector4s64" >;

// Vector 3
using GetVector3f32Components = GetVector3Components< Node,
                                                      float32,
                                                      "onyx::node_graph::nodes::GetVector3f32Components" >;
using GetVector3f64Components = GetVector3Components< Node,
                                                      float64,
                                                      "onyx::node_graph::nodes::GetVector3f64Components" >;
using GetVector3s32Components = GetVector3Components< Node,
                                                      int32_t,
                                                      "onyx::node_graph::nodes::GetVector3s32Components" >;
using GetVector3s64Components = GetVector3Components< Node,
                                                      int64_t,
                                                      "onyx::node_graph::nodes::GetVector3s64Components" >;

using CreateVector3f32 = CreateVector3< Node, float32, "onyx::node_graph::nodes::CreateVector3f32" >;
using CreateVector3f64 = CreateVector3< Node, float64, "onyx::node_graph::nodes::CreateVector3f64" >;
using CreateVector3s32 = CreateVector3< Node, int32_t, "onyx::node_graph::nodes::CreateVector3s32" >;
using CreateVector3s64 = CreateVector3< Node, int64_t, "onyx::node_graph::nodes::CreateVector3s64" >;

using Swizzle3DVector2f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f32,
                                                         Vector3f32,
                                                         "onyx::node_graph::nodes::Swizzle3DVector2f32" >;
using Swizzle3DVector3f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f32,
                                                         Vector3f32,
                                                         "onyx::node_graph::nodes::Swizzle3DVector3f32" >;
using Swizzle3DVector4f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f32,
                                                         Vector3f32,
                                                         "onyx::node_graph::nodes::Swizzle3DVector4f32" >;

using Swizzle3DVector2f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f64,
                                                         Vector3f64,
                                                         "onyx::node_graph::nodes::Swizzle3DVector2f64" >;
using Swizzle3DVector3f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f64,
                                                         Vector3f64,
                                                         "onyx::node_graph::nodes::Swizzle3DVector3f64" >;
using Swizzle3DVector4f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f64,
                                                         Vector3f64,
                                                         "onyx::node_graph::nodes::Swizzle3DVector4f64" >;

using Swizzle3DVector2s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s32,
                                                         Vector3s32,
                                                         "onyx::node_graph::nodes::Swizzle3DVector2s32" >;
using Swizzle3DVector3s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s32,
                                                         Vector3s32,
                                                         "onyx::node_graph::nodes::Swizzle3DVector3s32" >;
using Swizzle3DVector4s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s32,
                                                         Vector3s32,
                                                         "onyx::node_graph::nodes::Swizzle3DVector4s32" >;

using Swizzle3DVector2s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s64,
                                                         Vector3s64,
                                                         "onyx::node_graph::nodes::Swizzle3DVector2s64" >;
using Swizzle3DVector3s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s64,
                                                         Vector3s64,
                                                         "onyx::node_graph::nodes::Swizzle3DVector3s64" >;
using Swizzle3DVector4s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s64,
                                                         Vector3s64,
                                                         "onyx::node_graph::nodes::Swizzle3DVector4s64" >;

// Vector 4
using GetVector4f32Components = GetVector4Components< Node,
                                                      float32,
                                                      "onyx::node_graph::nodes::GetVector4f32Components" >;
using GetVector4f64Components = GetVector4Components< Node,
                                                      float64,
                                                      "onyx::node_graph::nodes::GetVector4f64Components" >;
using GetVector4s32Components = GetVector4Components< Node,
                                                      int32_t,
                                                      "onyx::node_graph::nodes::GetVector4s32Components" >;
using GetVector4s64Components = GetVector4Components< Node,
                                                      int64_t,
                                                      "onyx::node_graph::nodes::GetVector4s64Components" >;

using CreateVector4f32 = CreateVector4< Node, float32, "onyx::node_graph::nodes::CreateVector4f32" >;
using CreateVector4f64 = CreateVector4< Node, float64, "onyx::node_graph::nodes::CreateVector4f64" >;
using CreateVector4s32 = CreateVector4< Node, int32_t, "onyx::node_graph::nodes::CreateVector4s32" >;
using CreateVector4s64 = CreateVector4< Node, int64_t, "onyx::node_graph::nodes::CreateVector4s64" >;

using Swizzle4DVector2f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f32,
                                                         Vector4f32,
                                                         "onyx::node_graph::nodes::Swizzle4DVector2f32" >;
using Swizzle4DVector3f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f32,
                                                         Vector4f32,
                                                         "onyx::node_graph::nodes::Swizzle4DVector3f32" >;
using Swizzle4DVector4f32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f32,
                                                         Vector4f32,
                                                         "onyx::node_graph::nodes::Swizzle4DVector4f32" >;

using Swizzle4DVector2f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2f64,
                                                         Vector4f64,
                                                         "onyx::node_graph::nodes::Swizzle4DVector2f64" >;
using Swizzle4DVector3f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3f64,
                                                         Vector4f64,
                                                         "onyx::node_graph::nodes::Swizzle4DVector3f64" >;
using Swizzle4DVector4f64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4f64,
                                                         Vector4f64,
                                                         "onyx::node_graph::nodes::Swizzle4DVector4f64" >;

using Swizzle4DVector2s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s32,
                                                         Vector4s32,
                                                         "onyx::node_graph::nodes::Swizzle4DVector2s32" >;
using Swizzle4DVector3s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s32,
                                                         Vector4s32,
                                                         "onyx::node_graph::nodes::Swizzle4DVector3s32" >;
using Swizzle4DVector4s32 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s32,
                                                         Vector4s32,
                                                         "onyx::node_graph::nodes::Swizzle4DVector4s32" >;

using Swizzle4DVector2s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector2s64,
                                                         Vector4s64,
                                                         "onyx::node_graph::nodes::Swizzle4DVector2s64" >;
using Swizzle4DVector3s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector3s64,
                                                         Vector4s64,
                                                         "onyx::node_graph::nodes::Swizzle4DVector3s64" >;
using Swizzle4DVector4s64 = SwizzleVectorComponentsNode< Node,
                                                         Vector4s64,
                                                         Vector4s64,
                                                         "onyx::node_graph::nodes::Swizzle4DVector4s64" >;
} // namespace nodes

} // namespace onyx::node_graph
