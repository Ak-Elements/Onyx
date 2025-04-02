#pragma once

#include <onyx/nodegraph/node.h>
#include <onyx/geometry/vector.h>

#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Editor
{
    template <typename OutVectorT, typename InVectorT>
    struct EditorSwizzleVectorControl;

    // Float
    template <>
    struct EditorSwizzleVectorControl<Vector2f, Vector2f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2f, Vector3f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2f, Vector4f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f, Vector2f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f, Vector3f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3f, Vector4f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f, Vector2f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f, Vector3f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4f, Vector4f>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    // Double
    template <>
    struct EditorSwizzleVectorControl<Vector2d, Vector2d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2d, Vector3d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector2d, Vector4d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3d, Vector2d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3d, Vector3d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector3d, Vector4d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4d, Vector2d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4d, Vector3d>
    {
        static Optional<Vector4u8> Draw(SwizzleMask& mask);
    };

    template <>
    struct EditorSwizzleVectorControl<Vector4d, Vector4d>
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
