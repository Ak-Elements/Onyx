#pragma once
#include <onyx/nodegraph/nodes/math/vectornodes.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::node_graph {
template < typename InVectorT, typename OutVectorT, CompileTimeString TypeIdString >
class SwizzleVectorComponentsNode< graphics::ShaderGraphNode, InVectorT, OutVectorT, TypeIdString >
    : public FixedPinNode1In1Out< graphics::ShaderGraphNode, InVectorT, OutVectorT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In1Out< graphics::ShaderGraphNode, InVectorT, OutVectorT >;

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
        // Super::OnSerialize(json);
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
    bool onDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) override {
        bool hasModified = Super::onDrawInPropertyGrid( constantPinData );

        // Optional<Vector4u8> swizzle = Editor::EditorSwizzleVectorControl<InVectorT, OutVectorT>::Draw(Mask);
        // if (swizzle.has_value())
        //{
        //     hasModified = true;
        //     OutIndexMapping = swizzle.value();
        // }

        return hasModified;
    }

    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin& inputPin = Super::getInputPin();

        generator.appendCode( format::format(
            "{} pin_{:x} = {}.{}; \n",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< OutVectorT >().c_str(),
            Super::getOutputPin().getGlobalId().get(),
            inputPin.isConnected()
                ? format::format( "pin_{:x}", inputPin.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin >() ),
            toLower( enums::toString( m_mask ) ) ) );
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

template < typename ScalarT, CompileTimeString TypeIdString >
class GetVector2Components< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode1In2Out< graphics::ShaderGraphNode, Vector2< ScalarT >, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In2Out< graphics::ShaderGraphNode, Vector2< ScalarT >, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        Vector2< ScalarT > inPin = context.getPinData< typename Super::InPin >();

        ScalarT& out0 = context.getPinData< typename Super::OutPin0 >();
        out0 = inPin[ 0 ];

        ScalarT& out1 = context.getPinData< typename Super::OutPin1 >();
        out1 = inPin[ 1 ];
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin& inputPin = Super::getInputPin();
        if( inputPin.isConnected() ) {
            if( context.isPinConnected< typename Super::OutPin0 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.x; // X \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin0().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }

            if( context.isPinConnected< typename Super::OutPin1 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.y; // Y \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin1().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }
        } else {
            Vector2< ScalarT > inPin = context.getPinData< typename Super::InPin >();

            if( context.isPinConnected< typename Super::OutPin0 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // X \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin0().getGlobalId().get(),
                                    inPin[ 0 ] ) );
            }

            if( context.isPinConnected< typename Super::OutPin1 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // Y \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin1().getGlobalId().get(),
                                    inPin[ 1 ] ) );
            }
        }
    }

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

template < typename ScalarT, CompileTimeString TypeIdString >
class GetVector3Components< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode1In3Out< graphics::ShaderGraphNode, Vector3< ScalarT >, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In3Out< graphics::ShaderGraphNode, Vector3< ScalarT >, ScalarT, ScalarT, ScalarT >;

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
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin& inputPin = Super::getInputPin();

        if( inputPin.isConnected() ) {
            if( context.isPinConnected< typename Super::OutPin0 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.x; // X \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin0().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }

            if( context.isPinConnected< typename Super::OutPin1 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.y; // Y \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin1().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }

            if( context.isPinConnected< typename Super::OutPin2 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.z; // Z \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin2().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }
        } else {
            Vector3< ScalarT > inPin = context.getPinData< typename Super::InPin >();

            if( context.isPinConnected< typename Super::OutPin0 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // X \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin0().getGlobalId().get(),
                                    inPin[ 0 ] ) );
            }

            if( context.isPinConnected< typename Super::OutPin1 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // Y \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin1().getGlobalId().get(),
                                    inPin[ 1 ] ) );
            }

            if( context.isPinConnected< typename Super::OutPin2 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // Z \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin2().getGlobalId().get(),
                                    inPin[ 2 ] ) );
            }
        }
    }

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

template < typename ScalarT, CompileTimeString TypeIdString >
class GetVector4Components< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode1In4Out< graphics::ShaderGraphNode, Vector4< ScalarT >, ScalarT, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super =
        FixedPinNode1In4Out< graphics::ShaderGraphNode, Vector4< ScalarT >, ScalarT, ScalarT, ScalarT, ScalarT >;

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
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin& inputPin = Super::getInputPin();

        if( inputPin.isConnected() ) {
            if( context.isPinConnected< typename Super::OutPin0 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.x; // X \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin0().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }

            if( context.isPinConnected< typename Super::OutPin1 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.y; // Y \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin1().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }

            if( context.isPinConnected< typename Super::OutPin2 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.z; // Z \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin2().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }

            if( context.isPinConnected< typename Super::OutPin3 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = pin_{:x}.w; // W \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin3().getGlobalId().get(),
                                    inputPin.getLinkedPinGlobalId().get() ) );
            }
        } else {
            Vector4< ScalarT > inPin = context.getPinData< typename Super::InPin >();

            if( context.isPinConnected< typename Super::OutPin0 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // X \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin0().getGlobalId().get(),
                                    inPin[ 0 ] ) );
            }

            if( context.isPinConnected< typename Super::OutPin1 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // Y \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin1().getGlobalId().get(),
                                    inPin[ 1 ] ) );
            }

            if( context.isPinConnected< typename Super::OutPin2 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // Z \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin2().getGlobalId().get(),
                                    inPin[ 2 ] ) );
            }

            if( context.isPinConnected< typename Super::OutPin3 >() ) {
                generator.appendCode(
                    format::format( "{} pin_{:x} = {}; // W \n",
                                    rhi::ShaderGenerator::getTypeAsShaderTypeString< ScalarT >().c_str(),
                                    Super::getOutputPin3().getGlobalId().get(),
                                    inPin[ 3 ] ) );
            }
        }
    }

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

template < typename ScalarT, CompileTimeString TypeIdString >
class CreateVector2< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, Vector2< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, Vector2< ScalarT > >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.getPinData< typename Super::InPin1 >();

        Vector2< ScalarT >& out = context.getPinData< typename Super::OutPin >();
        out = Vector2( inPin0, inPin1 );
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();

        generator.appendCode( format::format(
            "{0} pin_{1:x} = {0}({2}, {3}); \n",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< Vector2< ScalarT > >().c_str(),
            Super::getOutputPin().getGlobalId().get(),
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );
    }

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

template < typename ScalarT, CompileTimeString TypeIdString >
class CreateVector3< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode3In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, Vector3< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode3In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, Vector3< ScalarT > >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT inPin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT inPin1 = context.getPinData< typename Super::InPin1 >();
        ScalarT inPin2 = context.getPinData< typename Super::InPin2 >();

        Vector3< ScalarT >& out = context.getPinData< typename Super::OutPin >();
        out = Vector3( inPin0, inPin1, inPin2 );
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();
        const typename Super::InPin2& inputPin2 = Super::getInputPin2();

        generator.appendCode( format::format(
            "{0} pin_{1:x} = {0}({2}, {3}, {4}); \n",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< Vector3< ScalarT > >().c_str(),
            Super::getOutputPin().getGlobalId().get(),
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ),
            inputPin2.isConnected()
                ? format::format( "pin_{:x}", inputPin2.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin2 >() ) ) );
    }

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

template < typename ScalarT, CompileTimeString TypeIdString >
class CreateVector4< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode4In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, ScalarT, Vector4< ScalarT > > {
  public:
    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super =
        FixedPinNode4In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT, ScalarT, Vector4< ScalarT > >;

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
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();
        const typename Super::InPin2& inputPin2 = Super::getInputPin2();
        const typename Super::InPin3& inputPin3 = Super::getInputPin3();

        generator.appendCode( format::format(
            "{0} pin_{1:x} = {0}({2}, {3}, {4}, {5}); \n",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< Vector4< ScalarT > >().c_str(),
            Super::getOutputPin().getGlobalId().get(),
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ),
            inputPin2.isConnected()
                ? format::format( "pin_{:x}", inputPin2.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin2 >() ),
            inputPin3.isConnected()
                ? format::format( "pin_{:x}", inputPin3.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin3 >() ) ) );
    }

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
} // namespace onyx::node_graph

namespace onyx::graphics::shader_graph_nodes {
using GetVector2f32Components = node_graph::
    GetVector2Components< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::GetVector2f32Components" >;
using GetVector2f64Components = node_graph::
    GetVector2Components< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::GetVector2f64Components" >;
using GetVector2s32Components = node_graph::
    GetVector2Components< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::GetVector2s32Components" >;
using GetVector2s64Components = node_graph::
    GetVector2Components< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::GetVector2s64Components" >;

using CreateVector2f32 = node_graph::
    CreateVector2< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::CreateVector2f32" >;
using CreateVector2f64 = node_graph::
    CreateVector2< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::CreateVector2f64" >;
using CreateVector2s32 = node_graph::
    CreateVector2< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::CreateVector2s32" >;
using CreateVector2s64 = node_graph::
    CreateVector2< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::CreateVector2s64" >;

using Swizzle2DVector2f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2f32,
    Vector2f32,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector2f32" >;
using Swizzle2DVector3f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3f32,
    Vector2f32,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector3f32" >;
using Swizzle2DVector4f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4f32,
    Vector2f32,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector4f32" >;

using Swizzle2DVector2f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2f64,
    Vector2f64,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector2f64" >;
using Swizzle2DVector3f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3f64,
    Vector2f64,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector3f64" >;
using Swizzle2DVector4f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4f64,
    Vector2f64,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector4f64" >;

using Swizzle2DVector2s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2s32,
    Vector2s32,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector2s32" >;
using Swizzle2DVector3s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3s32,
    Vector2s32,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector3s32" >;
using Swizzle2DVector4s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4s32,
    Vector2s32,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector4s32" >;

using Swizzle2DVector2s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2s64,
    Vector2s64,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector2s64" >;
using Swizzle2DVector3s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3s64,
    Vector2s64,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector3s64" >;
using Swizzle2DVector4s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4s64,
    Vector2s64,
    "onyx::graphics::shader_graph_nodes::Swizzle2DVector4s64" >;

// Vector 3
using GetVector3f32Components = node_graph::
    GetVector3Components< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::GetVector3f32Components" >;
using GetVector3f64Components = node_graph::
    GetVector3Components< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::GetVector3f64Components" >;
using GetVector3s32Components = node_graph::
    GetVector3Components< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::GetVector3s32Components" >;
using GetVector3s64Components = node_graph::
    GetVector3Components< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::GetVector3s64Components" >;

using CreateVector3f32 = node_graph::
    CreateVector3< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::CreateVector3f32" >;
using CreateVector3f64 = node_graph::
    CreateVector3< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::CreateVector3f64" >;
using CreateVector3s32 = node_graph::
    CreateVector3< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::CreateVector3s32" >;
using CreateVector3s64 = node_graph::
    CreateVector3< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::CreateVector3s64" >;

using Swizzle3DVector2f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2f32,
    Vector3f32,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector2f32" >;
using Swizzle3DVector3f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3f32,
    Vector3f32,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector3f32" >;
using Swizzle3DVector4f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4f32,
    Vector3f32,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector4f32" >;

using Swizzle3DVector2f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2f64,
    Vector3f64,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector2f64" >;
using Swizzle3DVector3f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3f64,
    Vector3f64,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector3f64" >;
using Swizzle3DVector4f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4f64,
    Vector3f64,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector4f64" >;

using Swizzle3DVector2s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2s32,
    Vector3s32,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector2s32" >;
using Swizzle3DVector3s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3s32,
    Vector3s32,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector3s32" >;
using Swizzle3DVector4s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4s32,
    Vector3s32,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector4s32" >;

using Swizzle3DVector2s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2s64,
    Vector3s64,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector2s64" >;
using Swizzle3DVector3s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3s64,
    Vector3s64,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector3s64" >;
using Swizzle3DVector4s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4s64,
    Vector3s64,
    "onyx::graphics::shader_graph_nodes::Swizzle3DVector4s64" >;

// Vector 4
using GetVector4f32Components = node_graph::
    GetVector4Components< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::GetVector4f32Components" >;
using GetVector4f64Components = node_graph::
    GetVector4Components< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::GetVector4f64Components" >;
using GetVector4s32Components = node_graph::
    GetVector4Components< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::GetVector4s32Components" >;
using GetVector4s64Components = node_graph::
    GetVector4Components< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::GetVector4s64Components" >;

using CreateVector4f32 = node_graph::
    CreateVector4< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::CreateVector4f32" >;
using CreateVector4f64 = node_graph::
    CreateVector4< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::CreateVector4f64" >;
using CreateVector4s32 = node_graph::
    CreateVector4< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::CreateVector4s32" >;
using CreateVector4s64 = node_graph::
    CreateVector4< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::CreateVector4s64" >;

using Swizzle4DVector2f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2f32,
    Vector4f32,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector2f32" >;
using Swizzle4DVector3f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3f32,
    Vector4f32,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector3f32" >;
using Swizzle4DVector4f32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4f32,
    Vector4f32,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector4f32" >;

using Swizzle4DVector2f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2f64,
    Vector4f64,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector2f64" >;
using Swizzle4DVector3f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3f64,
    Vector4f64,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector3f64" >;
using Swizzle4DVector4f64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4f64,
    Vector4f64,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector4f64" >;

using Swizzle4DVector2s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2s32,
    Vector4s32,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector2s32" >;
using Swizzle4DVector3s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3s32,
    Vector4s32,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector3s32" >;
using Swizzle4DVector4s32 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4s32,
    Vector4s32,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector4s32" >;

using Swizzle4DVector2s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector2s64,
    Vector4s64,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector2s64" >;
using Swizzle4DVector3s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector3s64,
    Vector4s64,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector3s64" >;
using Swizzle4DVector4s64 = node_graph::SwizzleVectorComponentsNode<
    ShaderGraphNode,
    Vector4s64,
    Vector4s64,
    "onyx::graphics::shader_graph_nodes::Swizzle4DVector4s64" >;
} // namespace onyx::graphics::shader_graph_nodes
