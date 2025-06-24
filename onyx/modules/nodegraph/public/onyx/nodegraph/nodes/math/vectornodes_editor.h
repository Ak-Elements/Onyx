#pragma once

#include <onyx/geometry/vector.h>

namespace Onyx::Editor
{
    template <typename OutVectorT, typename InVectorT>
    struct EditorSwizzleVectorControl;

    // Float
    template <>
    struct EditorSwizzleVectorControl<Vector2f32, Vector2f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2f32, Vector3f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2f32, Vector4f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f32, Vector2f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f32, Vector3f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f32, Vector4f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f32, Vector2f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f32, Vector3f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f32, Vector4f32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    // Double
    template <>
    struct EditorSwizzleVectorControl<Vector2f64, Vector2f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2f64, Vector3f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2f64, Vector4f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f64, Vector2f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f64, Vector3f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f64, Vector4f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f64, Vector2f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f64, Vector3f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f64, Vector4f64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    // S32
    template <>
    struct EditorSwizzleVectorControl<Vector2s32, Vector2s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2s32, Vector3s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2s32, Vector4s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3s32, Vector2s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3s32, Vector3s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3s32, Vector4s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4s32, Vector2s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4s32, Vector3s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4s32, Vector4s32>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    // S64
    template <>
    struct EditorSwizzleVectorControl<Vector2s64, Vector2s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2s64, Vector3s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2s64, Vector4s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3s64, Vector2s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3s64, Vector3s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3s64, Vector4s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4s64, Vector2s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4s64, Vector3s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4s64, Vector4s64>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };
}
