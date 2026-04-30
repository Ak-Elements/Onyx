#include <onyx/graphics/debug/debugdrawqueue.h>

namespace onyx::graphics {
void DebugDrawQueue::addWireframeSphere( Vector3f32 position, float32 radius, Color color ) {
    m_wireframeSpheres.emplace_back( position, radius, color );
}

void DebugDrawQueue::addWireframeBox( Vector3f32 position, Vector3f32 halfExtents, Color color ) {
    m_wireframeBoxes.emplace_back( position, color, Matrix3x3f32(), halfExtents );
}

void DebugDrawQueue::addWireframeBox( Vector3f32 position,
                                      Vector3f32 halfExtents,
                                      Matrix3x3f32 rotation,
                                      Color color ) {
    m_wireframeBoxes.emplace_back( position, color, rotation, halfExtents );
}

void DebugDrawQueue::clear() {
    m_wireframeBoxes.clear();
    m_wireframeSpheres.clear();
}
} // namespace onyx::graphics
