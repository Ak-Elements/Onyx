#pragma once

#include <onyx/graphics/debug/debugshapes.h>

namespace onyx::graphics {
class DebugDrawQueue {
  public:
    void addWireframeBox( Vector3f32 positon, Vector3f32 halfExtents, uint32_t color );
    void addWireframeBox( Vector3f32 positon, Vector3f32 halfExtents, Matrix3x3f32 rotation, uint32_t color );

    void addWireframeSphere( Vector3f32 positon, float32 radius, uint32_t color );

    void clear();

    Span< const DebugBox > GetWireframeBoxes() const { return Span< const DebugBox >( m_WireframeBoxes ); }
    Span< const DebugSphere > GetWireframeSpheres() const { return Span< const DebugSphere >( m_WireframeSpheres ); }

  private:
    DynamicArray< DebugBox > m_WireframeBoxes;
    DynamicArray< DebugSphere > m_WireframeSpheres;
};
} // namespace onyx::graphics
