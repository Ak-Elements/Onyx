#pragma once
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::assets
{
    class AssetSystem;
}
namespace onyx::rhi
{
    class ShaderGenerator;
}
namespace onyx::graphics
{
    class ShaderGraphNode : public node_graph::Node
    {
    public:
        void GenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const { DoGenerateShader(context, generator); }

        void OnNodeChanged(assets::AssetSystem& assetSystem) { OnChanged(assetSystem); }

    private:
        virtual void DoGenerateShader(const node_graph::ExecutionContext& /*context*/, rhi::ShaderGenerator& /*generator*/) const {}
        virtual void OnChanged(assets::AssetSystem& /*assetSystem*/) {}
    };
}
