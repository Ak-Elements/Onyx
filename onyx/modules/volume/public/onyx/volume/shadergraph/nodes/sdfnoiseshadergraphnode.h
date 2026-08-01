#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>
#include <onyx/nodegraph/nodes/fixedpinsnode.h>

namespace onyx::volume {
class SdfNoise2DShaderGraphNode : public node_graph::FixedPinNode< graphics::ShaderGraphNode, 2, 2 > {
  private:
    using NoiseValueInPin = node_graph::Pin< float32, "InPin0" >;
    using NoiseGradientInPin = node_graph::Pin< Vector2f32, "InPin1" >;
    using IsoValueOutPin = node_graph::Pin< float32, "OutPin0" >;
    using GradientOutPin = node_graph::Pin< Vector3f32, "OutPin1" >;

  public:
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfNoise2DShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    SdfNoise2DShaderGraphNode() = default;

    void onUpdate( node_graph::ExecutionContext& context ) const override;

    node_graph::PinBase* getInputPin( uint32_t index ) override;
    [[nodiscard]] const node_graph::PinBase* getInputPin( uint32_t index ) const override;

    node_graph::PinBase* getOutputPin( uint32_t index ) override;
    [[nodiscard]] const node_graph::PinBase* getOutputPin( uint32_t index ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif

  private:
    NoiseValueInPin m_noiseValueInPin;
    NoiseGradientInPin m_noiseGradientInPin;
    IsoValueOutPin m_isoValueOutPin;
    GradientOutPin m_gradientOutPin;
};

class SdfNoise3DShaderGraphNode : public node_graph::FixedPinNode< graphics::ShaderGraphNode, 2, 2 > {
  private:
    using NoiseValueInPin = node_graph::Pin< float32, "InPin0" >;
    using NoiseGradientInPin = node_graph::Pin< Vector3f32, "InPin1" >;
    using IsoValueOutPin = node_graph::Pin< float32, "OutPin0" >;
    using GradientOutPin = node_graph::Pin< Vector3f32, "OutPin1" >;

  public:
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfNoise3DShaderGraphNode";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    SdfNoise3DShaderGraphNode() = default;

    void onUpdate( node_graph::ExecutionContext& context ) const override;

    node_graph::PinBase* getInputPin( uint32_t index ) override;
    [[nodiscard]] const node_graph::PinBase* getInputPin( uint32_t index ) const override;

    node_graph::PinBase* getOutputPin( uint32_t index ) override;
    [[nodiscard]] const node_graph::PinBase* getOutputPin( uint32_t index ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  protected:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;

#endif
  private:
    NoiseValueInPin m_noiseValueInPin;
    NoiseGradientInPin m_noiseGradientInPin;
    IsoValueOutPin m_isoValueOutPin;
    GradientOutPin m_gradientOutPin;
};
} // namespace onyx::volume
