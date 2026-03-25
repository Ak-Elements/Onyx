#pragma once

#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/renderpass.h>

namespace onyx::rhi {
class GraphicsSystem;

class RenderPassCache {
  public:
    RenderPassCache( GraphicsSystem& graphicsSystem );
    void Clear();

    RenderPassHandle GetOrCreateRenderPass( const RenderPassSettings& settings );

  private:
    GraphicsSystem& m_GraphicsSystem;

    HashMap< uint64_t, RenderPassHandle > m_Cache;
};
} // namespace onyx::rhi
