#pragma once

#include <onyx/editor/nodegraph/typenodegrapheditorcontext.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>

namespace onyx::rhi {
class GraphicsSystem;
}

namespace onyx::editor {
class ShaderGraphEditorContext
    : public TypedNodeGraphEditorContext< graphics::MaterialShaderGraph, graphics::ShaderGraphNodeFactory > {
  public:
    ShaderGraphEditorContext( assets::AssetSystem& assetSystem, rhi::GraphicsSystem& graphicsSystem );

    bool Compile() override;

    uint32_t GetCanvasBackgroundColor() const override { return 0xFF503C46; }

  private:
    void OnNodeChanged( const Node& newNode ) override;

  private:
    assets::AssetSystem* m_AssetSystem = nullptr;
    rhi::GraphicsSystem* m_graphicsSystem = nullptr;
};

} // namespace onyx::editor
