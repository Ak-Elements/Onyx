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

    bool compile() override;

    uint32_t getCanvasBackgroundColor() const override { return 0xFF503C46; }

  private:
    void onNodeChanged( const Node& newNode ) override;

  private:
    assets::AssetSystem* m_AssetSystem = nullptr;
    rhi::GraphicsSystem* m_graphicsSystem = nullptr;
};

} // namespace onyx::editor
