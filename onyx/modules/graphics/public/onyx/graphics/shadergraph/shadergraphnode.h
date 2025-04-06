#pragma once
#include <onyx/nodegraph/node.h>

namespace Onyx::Graphics
{
    class ShaderGenerator;
}

namespace Onyx::Graphics
{
    class ShaderGraphNode : public NodeGraph::Node
    {
    public:
        void GenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const { DoGenerateShader(context, generator); }

        void OnNodeChanged(Assets::AssetSystem& assetSystem) { OnChanged(assetSystem); }

    private:
        virtual void DoGenerateShader(const NodeGraph::ExecutionContext& /*context*/, ShaderGenerator& /*generator*/) const {}
        virtual void OnChanged(Assets::AssetSystem& /*assetSystem*/) {}
    };
}
