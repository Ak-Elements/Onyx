#include <onyx/graphics/debug/debugdrawqueue.h>

namespace Onyx::Graphics
{
    DebugDrawQueue::DebugDrawQueue(DynamicArray<DebugSphere>& sphereQueue)
        : m_DebugSpheres(sphereQueue)
    {
    }

    void DebugDrawQueue::DrawSphere(Vector3f32 position, onyxF32 radius, onyxU32 color)
    {
        m_DebugSpheres.emplace_back(position, radius, color);
    }
}
