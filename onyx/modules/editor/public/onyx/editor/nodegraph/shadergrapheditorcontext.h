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
    bool compile() override;

    uint32_t getCanvasBackgroundColor() const override { return 0xFF503C46; }

  private:
    void onNodeChanged( const Node& newNode ) override;
};

} // namespace onyx::editor
