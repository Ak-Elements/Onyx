#pragma once

#include <onyx/editor/nodegraph/typenodegrapheditorcontext.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

namespace Onyx::Editor
{
    class VolumeShaderGraphEditorContext : public TypedNodeGraphEditorContext<Volume::VolumeShaderGraph, Graphics::ShaderGraphNodeFactory>
    {
    public:
        VolumeShaderGraphEditorContext(Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem);

        bool Compile() override;

        onyxU32 GetCanvasBackgroundColor() const override { return 0xFF503C46; }

    private:
        void OnNodeChanged(const Node& newNode) override;

    private:
        Assets::AssetSystem* m_AssetSystem = nullptr;
        Graphics::GraphicsSystem* m_GraphicsSystem = nullptr;
    };

}
