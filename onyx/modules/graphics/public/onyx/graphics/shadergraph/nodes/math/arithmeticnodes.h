#pragma once
#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics {
class ShaderGraphNode;
}

namespace onyx::node_graph {
template < typename ScalarT, CompileTimeString TypeIdString >
class AddNode< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr bool HasAliases = true;
    static constexpr bool ShowNodeName = false;

    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT pin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT pin1 = context.getPinData< typename Super::InPin1 >();

        ScalarT& out = context.getPinData< typename Super::OutPin >();
        out = pin0 + pin1;
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        generator.appendCode( "// Add node \n" );
        generator.appendCode( format::format(
            "{} pin_{:x} = ",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< typename Super::OutPin::DataType >().c_str(),
            Super::getOutputPin().getGlobalId().get() ) );

        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        generator.appendCode(
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ) );
        generator.appendCode( " + " );
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();
        generator.appendCode(
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) );
        generator.appendCode( "; \n" );
    }

    void onUiDrawNodeBackground() override {
        // DrawPlusIconBackground(Super::GetId());
    }

    [[nodiscard]] StringView getPinName( StringId32 /*pinId*/ ) const override {
        // this node should not show any labels
        return "";
    }
#endif
};

template < typename ScalarT, CompileTimeString TypeIdString >
class SubtractNode< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr bool HasAliases = true;
    static constexpr bool ShowNodeName = false;

    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT pin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT pin1 = context.getPinData< typename Super::InPin1 >();

        ScalarT& out = context.getPinData< typename Super::OutPin >();
        out = pin0 - pin1;
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        generator.appendCode( "// Subtract node \n" );
        generator.appendCode( format::format(
            "{} pin_{:x} = ",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< typename Super::OutPin::DataType >().c_str(),
            Super::getOutputPin().getGlobalId().get() ) );

        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        generator.appendCode(
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ) );
        generator.appendCode( " - " );
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();
        generator.appendCode(
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) );
        generator.appendCode( "; \n" );
    }

    void onUiDrawNodeBackground() override {
        // DrawMinusIconBackground(Super::GetId());
    }

    [[nodiscard]] StringView getPinName( StringId32 /*pinId*/ ) const override {
        // this node should not show any labels
        return "";
    }
#endif
};

template < typename ScalarT, CompileTimeString TypeIdString >
class MultiplyNode< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr bool HasAliases = true;
    static constexpr bool ShowNodeName = false;

    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT pin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT pin1 = context.getPinData< typename Super::InPin1 >();

        ScalarT& out = context.getPinData< typename Super::OutPin >();
        out = pin0 * pin1;
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        generator.appendCode( "// Multiply node \n" );
        generator.appendCode( format::format(
            "{} pin_{:x} = ",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< typename Super::OutPin::DataType >().c_str(),
            Super::getOutputPin().getGlobalId().get() ) );

        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        generator.appendCode(
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ) );
        generator.appendCode( " * " );
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();
        generator.appendCode(
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) );
        generator.appendCode( "; \n" );
    }

    void onUiDrawNodeBackground() override {
        // DrawMultiplyIconBackground(Super::GetId());
    }

    [[nodiscard]] StringView getPinName( StringId32 /*pinId*/ ) const override {
        // this node should not show any labels
        return "";
    }
#endif
};

template < typename ScalarT, CompileTimeString TypeIdString >
class DivisionNode< graphics::ShaderGraphNode, ScalarT, TypeIdString >
    : public FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT > {
  public:
    static constexpr bool HasAliases = true;
    static constexpr bool ShowNodeName = false;

    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode2In1Out< graphics::ShaderGraphNode, ScalarT, ScalarT, ScalarT >;

    void onUpdate( ExecutionContext& context ) const override {
        ScalarT pin0 = context.getPinData< typename Super::InPin0 >();
        ScalarT pin1 = context.getPinData< typename Super::InPin1 >();

        ScalarT& out = context.getPinData< typename Super::OutPin >();
        out = pin0 / pin1;
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        generator.appendCode( "// Division node \n" );
        generator.appendCode( format::format(
            "{} pin_{:x} = ",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< typename Super::OutPin::DataType >().c_str(),
            Super::getOutputPin().getGlobalId().get() ) );

        const typename Super::InPin0& inputPin0 = Super::getInputPin0();
        generator.appendCode(
            inputPin0.isConnected()
                ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ) );
        generator.appendCode( " / " );
        const typename Super::InPin1& inputPin1 = Super::getInputPin1();
        generator.appendCode(
            inputPin1.isConnected()
                ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) );
        generator.appendCode( "; \n" );
    }

    void onUiDrawNodeBackground() override {
        // DrawDivisionIconBackground(Super::GetId());
    }

    [[nodiscard]] StringView getPinName( StringId32 /*pinId*/ ) const override {
        // this node should not show any labels
        return "";
    }
#endif
};

template < typename DataT, CompileTimeString TypeIdString >
class AbsoluteNode< graphics::ShaderGraphNode, DataT, TypeIdString >
    : public FixedPinNode1In1Out< graphics::ShaderGraphNode, DataT, DataT > {
  public:
    static constexpr bool HasAliases = true;
    static constexpr bool ShowNodeName = false;

    static constexpr StringId32 TypeId = TypeIdString;
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    using Super = FixedPinNode1In1Out< graphics::ShaderGraphNode, DataT, DataT >;

    void onUpdate( ExecutionContext& context ) const override {
        DataT inPinValue = context.getPinData< typename Super::InPin >();
        DataT& outPinValue = context.getPinData< typename Super::OutPin >();

        if constexpr( is_specialization_of_v< Vector4, DataT > ) {
            outPinValue = DataT( std::abs( inPinValue[ 0 ] ),
                                 std::abs( inPinValue[ 1 ] ),
                                 std::abs( inPinValue[ 2 ] ),
                                 std::abs( inPinValue[ 3 ] ) );
        } else if constexpr( is_specialization_of_v< Vector3, DataT > ) {
            outPinValue = DataT( std::abs( inPinValue[ 0 ] ),
                                 std::abs( inPinValue[ 1 ] ),
                                 std::abs( inPinValue[ 2 ] ) );
        } else if constexpr( is_specialization_of_v< Vector2, DataT > ) {
            outPinValue = DataT( std::abs( inPinValue[ 0 ] ), std::abs( inPinValue[ 1 ] ) );
        } else if constexpr( std::is_integral_v< DataT > || std::is_floating_point_v< DataT > ) {
            outPinValue = std::abs( inPinValue );
        }
    }

#if ONYX_IS_EDITOR
  private:
    void doGenerateShader( const ExecutionContext& context, rhi::ShaderGenerator& generator ) const override {
        const typename Super::InPin& inputPin = Super::getInputPin();
        generator.appendCode( "// Absolute node \n" );
        generator.appendCode( format::format(
            "{} pin_{:x} = abs({}); \n",
            rhi::ShaderGenerator::getTypeAsShaderTypeString< typename Super::OutPin::DataType >().c_str(),
            Super::getOutputPin().getGlobalId().get(),
            inputPin.isConnected()
                ? format::format( "pin_{:x}", inputPin.getLinkedPinGlobalId().get() )
                : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin >() ) ) );
    }

    void onUiDrawNodeBackground() override {
        // DrawAbsoluteIconBackground(Super::GetId());
    }

    [[nodiscard]] StringView getPinName( StringId32 /*pinId*/ ) const override {
        // this node should not show any labels
        return "";
    }
#endif
};
} // namespace onyx::node_graph

namespace onyx::graphics::shader_graph_nodes {
// Addition
using AddNodeF32 = node_graph::AddNode< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::AdditionF32" >;
using AddNodeF64 = node_graph::AddNode< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::AdditionF64" >;
using AddNodeS32 = node_graph::AddNode< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::AdditionS32" >;
using AddNodeS64 = node_graph::AddNode< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::AdditionS64" >;
using AddNodeVector2f32 = node_graph::
    AddNode< ShaderGraphNode, Vector2f32, "onyx::graphics::shader_graph_nodes::AdditionVector2f32" >;
using AddNodeVector2f64 = node_graph::
    AddNode< ShaderGraphNode, Vector2f64, "onyx::graphics::shader_graph_nodes::AdditionVector2f64" >;
using AddNodeVector2s32 = node_graph::
    AddNode< ShaderGraphNode, Vector2s32, "onyx::graphics::shader_graph_nodes::AdditionVector2s32" >;
using AddNodeVector2s64 = node_graph::
    AddNode< ShaderGraphNode, Vector2s64, "onyx::graphics::shader_graph_nodes::AdditionVector2s64" >;
using AddNodeVector3f32 = node_graph::
    AddNode< ShaderGraphNode, Vector3f32, "onyx::graphics::shader_graph_nodes::AdditionVector3f32" >;
using AddNodeVector3f64 = node_graph::
    AddNode< ShaderGraphNode, Vector3f64, "onyx::graphics::shader_graph_nodes::AdditionVector3f64" >;
using AddNodeVector3s32 = node_graph::
    AddNode< ShaderGraphNode, Vector3s32, "onyx::graphics::shader_graph_nodes::AdditionVector3s32" >;
using AddNodeVector3s64 = node_graph::
    AddNode< ShaderGraphNode, Vector3s64, "onyx::graphics::shader_graph_nodes::AdditionVector3s64" >;
using AddNodeVector4f32 = node_graph::
    AddNode< ShaderGraphNode, Vector4f32, "onyx::graphics::shader_graph_nodes::AdditionVector4f32" >;
using AddNodeVector4f64 = node_graph::
    AddNode< ShaderGraphNode, Vector4f64, "onyx::graphics::shader_graph_nodes::AdditionVector4f64" >;
using AddNodeVector4s32 = node_graph::
    AddNode< ShaderGraphNode, Vector4s32, "onyx::graphics::shader_graph_nodes::AdditionVector4s32" >;
using AddNodeVector4s64 = node_graph::
    AddNode< ShaderGraphNode, Vector4s64, "onyx::graphics::shader_graph_nodes::AdditionVector4s64" >;

// Subtraction
using SubtractNodeF32 = node_graph::
    SubtractNode< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::SubtractF32" >;
using SubtractNodeF64 = node_graph::
    SubtractNode< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::SubtractF64" >;
using SubtractNodeS32 = node_graph::
    SubtractNode< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::SubtractS32" >;
using SubtractNodeS64 = node_graph::
    SubtractNode< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::SubtractS64" >;
using SubtractNodeVector2f32 = node_graph::
    SubtractNode< ShaderGraphNode, Vector2f32, "onyx::graphics::shader_graph_nodes::SubtractVector2f32" >;
using SubtractNodeVector2f64 = node_graph::
    SubtractNode< ShaderGraphNode, Vector2f64, "onyx::graphics::shader_graph_nodes::SubtractVector2f64" >;
using SubtractNodeVector2s32 = node_graph::
    SubtractNode< ShaderGraphNode, Vector2s32, "onyx::graphics::shader_graph_nodes::SubtractVector2s32" >;
using SubtractNodeVector2s64 = node_graph::
    SubtractNode< ShaderGraphNode, Vector2s64, "onyx::graphics::shader_graph_nodes::SubtractVector2s64" >;
using SubtractNodeVector3f32 = node_graph::
    SubtractNode< ShaderGraphNode, Vector3f32, "onyx::graphics::shader_graph_nodes::SubtractVector3f32" >;
using SubtractNodeVector3f64 = node_graph::
    SubtractNode< ShaderGraphNode, Vector3f64, "onyx::graphics::shader_graph_nodes::SubtractVector3f64" >;
using SubtractNodeVector3s32 = node_graph::
    SubtractNode< ShaderGraphNode, Vector3s32, "onyx::graphics::shader_graph_nodes::SubtractVector3s32" >;
using SubtractNodeVector3s64 = node_graph::
    SubtractNode< ShaderGraphNode, Vector3s64, "onyx::graphics::shader_graph_nodes::SubtractVector3s64" >;
using SubtractNodeVector4f32 = node_graph::
    SubtractNode< ShaderGraphNode, Vector4f32, "onyx::graphics::shader_graph_nodes::SubtractVector4f32" >;
using SubtractNodeVector4f64 = node_graph::
    SubtractNode< ShaderGraphNode, Vector4f64, "onyx::graphics::shader_graph_nodes::SubtractVector4f64" >;
using SubtractNodeVector4s32 = node_graph::
    SubtractNode< ShaderGraphNode, Vector4s32, "onyx::graphics::shader_graph_nodes::SubtractVector4s32" >;
using SubtractNodeVector4s64 = node_graph::
    SubtractNode< ShaderGraphNode, Vector4s64, "onyx::graphics::shader_graph_nodes::SubtractVector4s64" >;

// Multiplication
using MultiplyNodeF32 = node_graph::
    MultiplyNode< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::MultiplyF32" >;
using MultiplyNodeF64 = node_graph::
    MultiplyNode< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::MultiplyF64" >;
using MultiplyNodeS32 = node_graph::
    MultiplyNode< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::MultiplyS32" >;
using MultiplyNodeS64 = node_graph::
    MultiplyNode< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::MultiplyS64" >;

// Division
using DivisionNodeF32 = node_graph::
    DivisionNode< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::DivideF32" >;
using DivisionNodeF64 = node_graph::
    DivisionNode< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::DivideF64" >;
using DivisionNodeS32 = node_graph::
    DivisionNode< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::DivideS32" >;
using DivisionNodeS64 = node_graph::
    DivisionNode< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::DivideS64" >;

using AbsoluteNodeF32 = node_graph::
    AbsoluteNode< ShaderGraphNode, float32, "onyx::graphics::shader_graph_nodes::AbsoluteF32" >;
using AbsoluteNodeF64 = node_graph::
    AbsoluteNode< ShaderGraphNode, float64, "onyx::graphics::shader_graph_nodes::AbsoluteF64" >;
using AbsoluteNodeS32 = node_graph::
    AbsoluteNode< ShaderGraphNode, int32_t, "onyx::graphics::shader_graph_nodes::AbsoluteS32" >;
using AbsoluteNodeS64 = node_graph::
    AbsoluteNode< ShaderGraphNode, int64_t, "onyx::graphics::shader_graph_nodes::AbsoluteS64" >;
using AbsoluteNodeVector2f32 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector2f32, "onyx::graphics::shader_graph_nodes::AbsoluteVector2f32" >;
using AbsoluteNodeVector2f64 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector2f64, "onyx::graphics::shader_graph_nodes::AbsoluteVector2f64" >;
using AbsoluteNodeVector2s32 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector2s32, "onyx::graphics::shader_graph_nodes::AbsoluteVector2s32" >;
using AbsoluteNodeVector2s64 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector2s64, "onyx::graphics::shader_graph_nodes::AbsoluteVector2s64" >;
using AbsoluteNodeVector3f32 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector3f32, "onyx::graphics::shader_graph_nodes::AbsoluteVector3f32" >;
using AbsoluteNodeVector3f64 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector3f64, "onyx::graphics::shader_graph_nodes::AbsoluteVector3f64" >;
using AbsoluteNodeVector3s32 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector3s32, "onyx::graphics::shader_graph_nodes::AbsoluteVector3s32" >;
using AbsoluteNodeVector3s64 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector3s64, "onyx::graphics::shader_graph_nodes::AbsoluteVector3s64" >;
using AbsoluteNodeVector4f32 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector4f32, "onyx::graphics::shader_graph_nodes::AbsoluteVector4f32" >;
using AbsoluteNodeVector4f64 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector4f64, "onyx::graphics::shader_graph_nodes::AbsoluteVector4f64" >;
using AbsoluteNodeVector4s32 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector4s32, "onyx::graphics::shader_graph_nodes::AbsoluteVector4s32" >;
using AbsoluteNodeVector4s64 = node_graph::
    AbsoluteNode< ShaderGraphNode, Vector4s64, "onyx::graphics::shader_graph_nodes::AbsoluteVector4s64" >;
} // namespace onyx::graphics::shader_graph_nodes
