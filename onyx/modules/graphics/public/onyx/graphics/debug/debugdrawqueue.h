#pragma once

#include <onyx/graphics/debug/debugshapes.h>

namespace Onyx::Graphics
{
    class DebugDrawQueue
    {
    public:
        void addWireframeBox(Vector3f32 positon, Vector3f32 halfExtents, onyxU32 color); 
        void addWireframeBox(Vector3f32 positon, Vector3f32 halfExtents, Matrix3x3f32 rotation, onyxU32 color);

        void addWireframeSphere(Vector3f32 positon, onyxF32 radius, onyxU32 color);

        void clear();

        Span<const DebugBox> GetWireframeBoxes() const { return Span<const DebugBox>(m_WireframeBoxes); }
        Span<const DebugSphere> GetWireframeSpheres() const { return Span<const DebugSphere>(m_WireframeSpheres); }

    private:
        DynamicArray<DebugBox> m_WireframeBoxes;
        DynamicArray<DebugSphere> m_WireframeSpheres;
    };
}
