#pragma once
#include <onyx/nodegraph/nodes/node.h>

namespace onyx::assets {
class AssetSystem;
}
namespace onyx::rhi {
class ShaderGenerator;
}
namespace onyx::graphics {
class ShaderGraphNode : public node_graph::Node {
  public:
    void generateShader( const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator ) const {
        doGenerateShader( context, generator );
    }

    void onNodeChanged( assets::AssetSystem& assetSystem ) { onChanged( assetSystem ); }

  private:
    virtual void doGenerateShader( const node_graph::ExecutionContext& /*context*/,
                                   rhi::ShaderGenerator& /*generator*/ ) const {}
    virtual void onChanged( assets::AssetSystem& /*assetSystem*/ ) {}
};
} // namespace onyx::graphics
