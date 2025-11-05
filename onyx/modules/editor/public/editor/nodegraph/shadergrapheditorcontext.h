#pragma once

#include <editor/nodegraph/typenodegrapheditorcontext.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>

namespace Onyx::Graphics
{
    class GraphicsSystem;
}

namespace Onyx::Editor
{
    class ShaderGraphEditorContext : public TypedNodeGraphEditorContext<Graphics::MaterialShaderGraph, Graphics::ShaderGraphNodeFactory>
    {
    public:
        ShaderGraphEditorContext(Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem);

        bool Compile() override;

        onyxU32 GetCanvasBackgroundColor() const override { return 0xFF503C46; }

    private:
        void OnNodeChanged(const Node& newNode) override;

    private:
        Assets::AssetSystem* m_AssetSystem = nullptr;
        Graphics::GraphicsSystem* m_GraphicsSystem = nullptr;
    };

}
