#pragma once

#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in.h>
#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace onyx::graphics::shader_graph_nodes {
class FragmentShaderOutNode : public node_graph::FixedPinNode1In< ShaderGraphNode, Vector4f32 > {
  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::FragmentShaderOut";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
  private:
    [[nodiscard]] StringView getPinName( StringId32 pinId ) const override;
#endif
};

class PBRMaterialShaderOutNode : public node_graph::FixedPinNode< ShaderGraphNode, 4, 0 > {
  private:
    using AlbedoInPin = node_graph::Pin< Vector3f32, "Albedo" >;
    using NormalInPin = node_graph::Pin< Vector3f32, "Normal" >;
    using MetalnessInPin = node_graph::Pin< float32, "Metalness" >;
    using RoughnessInPin = node_graph::Pin< float32, "Roughness" >;

    using Super = node_graph::FixedPinNode< ShaderGraphNode, 4, 0 >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::PBRMaterialShaderOut";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    node_graph::PinBase* getInputPin( uint32_t index ) override;
    [[nodiscard]] const node_graph::PinBase* getInputPin( uint32_t index ) const override;

    node_graph::PinBase* getOutputPin( uint32_t index ) override;
    [[nodiscard]] const node_graph::PinBase* getOutputPin( uint32_t index ) const override;

  private:
    void doGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;

#if ONYX_IS_EDITOR
    [[nodiscard]] std::any createDefaultForPin( StringId32 pinId ) const override;
#endif
  private:
    AlbedoInPin m_albedoInPin;
    NormalInPin m_normalInPin;
    MetalnessInPin m_metalnessInPin;
    RoughnessInPin m_roughnessInPin;
};
} // namespace onyx::graphics::shader_graph_nodes
