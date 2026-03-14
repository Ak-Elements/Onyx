#include <onyx/graphics/debug/debugdrawqueue.h>

namespace Onyx::Graphics
{
    void DebugDrawQueue::addWireframeSphere(Vector3f32 position, onyxF32 radius, onyxU32 color)
    {
        m_WireframeSpheres.emplace_back(position, radius, color);
    }

    void DebugDrawQueue::addWireframeBox(Vector3f32 position, Vector3f32 halfExtents, onyxU32 color)
    {
        m_WireframeBoxes.emplace_back(position, color, Matrix3x3f32(), halfExtents);
    }

    void DebugDrawQueue::addWireframeBox(Vector3f32 position, Vector3f32 halfExtents, Matrix3x3f32 rotation, onyxU32 color)
    {
        m_WireframeBoxes.emplace_back(position, color, rotation, halfExtents);
    }

    void DebugDrawQueue::clear()
    {
        m_WireframeBoxes.clear();
        m_WireframeSpheres.clear();
    }
}
