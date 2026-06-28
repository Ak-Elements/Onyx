#pragma once

#include <onyx/graphics/rendergraph/rendergraph.h> // TODO: REMOVE
#include <onyx/graphics/rendergraph/rendergraphtask.h>
#include <onyx/nodegraph/nodes/fixedpinnode2in1out.h>

namespace onyx::volume {

struct VolumeTerrainInstance {
    rhi::ShaderInstanceHandle Shader;
    rhi::BufferHandle VolumeSources;
    rhi::BufferHandle VolumeSourcesData;
};

class VolumeTerrainPass : public node_graph::FixedPinNode_2_In_1_Out< graphics::RenderGraphShaderNode,
                                                                      rhi::TextureHandle,
                                                                      rhi::TextureHandle,
                                                                      rhi::TextureHandle > {
  public:
    static constexpr StringId32 TypeId = "onyx::volume::render_graph::VolumeTerrainPass";
    StringId32 GetTypeId() const override { return TypeId; }

    VolumeTerrainPass();

    bool isEnabled() const override { return true; }

  private:
    using Super = node_graph::FixedPinNode_2_In_1_Out< graphics::RenderGraphShaderNode,
                                                       rhi::TextureHandle,
                                                       rhi::TextureHandle,
                                                       rhi::TextureHandle >;

    void onBeginFrame( graphics::RenderGraphContext& ) override;
    void onRender( graphics::RenderGraphContext& context, rhi::CommandBuffer& commandBuffer ) override;

#if ONYX_IS_EDITOR
  private:
    StringView GetPinName( StringId32 pinId ) const override {
        switch( pinId ) {
        case Super::InPin0::LocalId:
            return "Render Target";
        case Super::InPin1::LocalId:
            return "Depth Target";
        case Super::OutPin::LocalId:
            return "Out";
        }

        ONYX_ASSERT( false, "Invalid pin id" );
        return "";
    }
#endif
};
} // namespace onyx::volume
