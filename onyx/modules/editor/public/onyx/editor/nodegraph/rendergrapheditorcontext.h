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
    bool compile() override;
    uint32_t getCanvasBackgroundColor() const override { return 0xFF50463C; }
};
} // namespace onyx::editor
