#include <onyx/graphics/debug/debugdrawqueue.h>

namespace onyx::graphics {
void DebugDrawQueue::addWireframeSphere( Vector3f32 position, float32 radius, uint32_t color ) {
    m_WireframeSpheres.emplace_back( position, radius, color );
}

void DebugDrawQueue::addWireframeBox( Vector3f32 position, Vector3f32 halfExtents, uint32_t color ) {
    m_WireframeBoxes.emplace_back( position, color, Matrix3x3f32(), halfExtents );
}

void DebugDrawQueue::addWireframeBox( Vector3f32 position,
                                      Vector3f32 halfExtents,
                                      Matrix3x3f32 rotation,
                                      uint32_t color ) {
    m_WireframeBoxes.emplace_back( position, color, rotation, halfExtents );
}

void DebugDrawQueue::clear() {
    m_WireframeBoxes.clear();
    m_WireframeSpheres.clear();
}
} // namespace onyx::graphics
