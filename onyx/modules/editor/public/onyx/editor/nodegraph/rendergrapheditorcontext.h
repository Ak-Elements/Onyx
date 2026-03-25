#pragma once

#include <onyx/editor/nodegraph/typenodegrapheditorcontext.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

namespace onyx::graphics {
class RenderGraph;
}

namespace onyx::editor {
class RenderGraphEditorContext
    : public TypedNodeGraphEditorContext< graphics::RenderGraph, graphics::RenderGraphNodeFactory > {
  public:
    bool Compile() override;
    uint32_t GetCanvasBackgroundColor() const override { return 0xFF50463C; }
};
} // namespace onyx::editor
