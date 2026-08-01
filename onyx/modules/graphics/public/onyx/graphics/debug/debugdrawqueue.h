#pragma once

#include <onyx/graphics/debug/debugshapes.h>

namespace onyx::graphics {
class DebugDrawQueue {
  public:
    void addWireframeBox( Vector3f32 position, Vector3f32 halfExtents, Color color );
    void addWireframeBox( Vector3f32 position, Vector3f32 halfExtents, Matrix3x3f32 rotation, Color color );

    void addWireframeSphere( Vector3f32 position, float32 radius, Color color );

    void addWireframeCapsule( Vector3f32 position, float32 radius, float32 halfHeight, Color color );
    void addWireframeCapsule( Vector3f32 position,
                              float32 radius,
                              float32 halfHeight,
                              Matrix3x3f32 rotation,
                              Color color );

    void clear();

    [[nodiscard]] Span< const DebugBox > getWireframeBoxes() const { return { m_wireframeBoxes }; }
    [[nodiscard]] Span< const DebugCapsule > getWireframeCapsules() const { return { m_wireframeCapsules }; }
    [[nodiscard]] Span< const DebugSphere > getWireframeSpheres() const { return { m_wireframeSpheres }; }

  private:
    DynamicArray< DebugBox > m_wireframeBoxes;
    DynamicArray< DebugCapsule > m_wireframeCapsules;
    DynamicArray< DebugSphere > m_wireframeSpheres;
};
} // namespace onyx::graphics
