#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace onyx::volume {
class SdfUnionVolumeShaderGraphNode : public node_graph::FixedPinNode< graphics::ShaderGraphNode, 4, 2 > {
  private:
    using Super = node_graph::FixedPinNode< graphics::ShaderGraphNode, 4, 2 >;

    using InPinIsoValueSource0 = node_graph::Pin< float32, "InPin0" >;
    using InPinGradientSource0 = node_graph::Pin< Vector3f32, "InPin1" >;
    using InPinIsoValueSource1 = node_graph::Pin< float32, "InPin2" >;
    using InPinGradientSource1 = node_graph::Pin< Vector3f32, "InPin3" >;

    using OutPinIsoValue = node_graph::Pin< float32, "OutPin0" >;
    using OutPinGradient = node_graph::Pin< Vector3f32, "OutPin1" >;

  public:
    static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfUnionVolumeShaderGraphNode";
    StringId32 GetTypeId() const override { return TypeId; }

    SdfUnionVolumeShaderGraphNode() = default;

    void OnUpdate( node_graph::ExecutionContext& context ) const override;

    node_graph::PinBase* GetInputPin( uint32_t index ) override;
    const node_graph::PinBase* GetInputPin( uint32_t index ) const override;

    node_graph::PinBase* GetOutputPin( uint32_t index ) override;
    const node_graph::PinBase* GetOutputPin( uint32_t index ) const override;

  private:
    void DoGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
    StringView GetPinName( StringId32 pinId ) const override;
#endif

  private:
    InPinIsoValueSource0 m_IsoValueSource0InPin;
    InPinGradientSource0 m_GradientSource0InPin;
    InPinIsoValueSource1 m_IsoValueSource1InPin;
    InPinGradientSource1 m_GradientSource1InPin;
    OutPinIsoValue m_IsoValueOutPin;
    OutPinGradient m_GradientOutPin;
};
} // namespace onyx::volume
