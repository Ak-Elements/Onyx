#pragma once

#include <editor/nodegraph/nodegrapheditorcontext.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

namespace Onyx::Graphics
{
    class RenderGraph;
}

namespace Onyx::Editor
{
    class RenderGraphEditorContext : public TypedNodeGraphEditorContext<Graphics::RenderGraph, Graphics::RenderGraphNodeFactory>
    {
    public:
        RenderGraphEditorContext();

        bool Compile() override;
    };
}
