#pragma once

#include <onyx/graphics/debug/debugshapes.h>

namespace Onyx::Graphics
{
    class DebugDrawQueue
    {
    public:
        DebugDrawQueue(DynamicArray<DebugSphere>& sphereQueue);

        void DrawSphere(Vector3f32 positon, onyxF32 radius, onyxU32 color);

    private:
        DynamicArray<DebugSphere>& m_DebugSpheres;
    };
}
