#pragma once

#include <onyx/editor/nodegraph/typenodegrapheditorcontext.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

namespace onyx::editor {
class VolumeShaderGraphEditorContext
    : public TypedNodeGraphEditorContext< volume::VolumeShaderGraph, graphics::ShaderGraphNodeFactory > {
  public:
    bool compile() override;

    uint32_t getCanvasBackgroundColor() const override { return 0xFF503C46; }

  private:
    void onNodeChanged( const Node& newNode ) override;
};

} // namespace onyx::editor
