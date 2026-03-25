#pragma once

#include <onyx/editor/nodegraph/typenodegrapheditorcontext.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

namespace onyx::editor {
class VolumeShaderGraphEditorContext
    : public TypedNodeGraphEditorContext< volume::VolumeShaderGraph, graphics::ShaderGraphNodeFactory > {
  public:
    VolumeShaderGraphEditorContext( assets::AssetSystem& assetSystem, rhi::GraphicsSystem& graphicsSystem );

    bool Compile() override;

    uint32_t GetCanvasBackgroundColor() const override { return 0xFF503C46; }

  private:
    void OnNodeChanged( const Node& newNode ) override;

  private:
    assets::AssetSystem* m_AssetSystem = nullptr;
    rhi::GraphicsSystem* m_GraphicsSystem = nullptr;
};

} // namespace onyx::editor
