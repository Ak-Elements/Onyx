#include <onyx/graphics/shadergraph/nodes/math/vectornodes.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::Editor
{
    namespace 
    {
        Vector4u8 ConvertMaskToVector(SwizzleMask mask)
        {
            switch (mask)
            {
                // 2D Swizzle
                case SwizzleMask::XY: return Vector4u8(0, 1, 0, 0);
                case SwizzleMask::XZ: return Vector4u8(0, 2, 0, 0);
                case SwizzleMask::XW: return Vector4u8(0, 3, 0, 0);
                case SwizzleMask::XX: return Vector4u8(0, 0, 0, 0);
                case SwizzleMask::YX: return Vector4u8(1, 0, 0, 0);
                case SwizzleMask::YY: return Vector4u8(1, 1, 0, 0);
                case SwizzleMask::YZ: return Vector4u8(1, 2, 0, 0);
                case SwizzleMask::YW: return Vector4u8(1, 3, 0, 0);
                case SwizzleMask::ZX: return Vector4u8(2, 0, 0, 0);
                case SwizzleMask::ZY: return Vector4u8(2, 1, 0, 0);
                case SwizzleMask::ZZ: return Vector4u8(2, 2, 0, 0);
                case SwizzleMask::ZW: return Vector4u8(2, 3, 0, 0);
                case SwizzleMask::WX: return Vector4u8(3, 0, 0, 0);
                case SwizzleMask::WY: return Vector4u8(3, 1, 0, 0);
                case SwizzleMask::WZ: return Vector4u8(3, 2, 0, 0);
                case SwizzleMask::WW: return Vector4u8(3, 3, 0, 0);

                // 3D Swizzle
                case SwizzleMask::XXX: return Vector4u8{ 0, 0, 0, 0 };
                case SwizzleMask::XXY: return Vector4u8{ 0, 0, 1, 0 };
                case SwizzleMask::XYX: return Vector4u8{ 0, 1, 0, 0 };
                case SwizzleMask::XYY: return Vector4u8{ 0, 1, 1, 0 };
                case SwizzleMask::YXX: return Vector4u8{ 1, 0, 0, 0 };
                case SwizzleMask::YYX: return Vector4u8{ 1, 1, 0, 0 };
                case SwizzleMask::YXY: return Vector4u8{ 1, 0, 1, 0 };
                case SwizzleMask::YYY: return Vector4u8{ 1, 1, 1, 0 };
                case SwizzleMask::XYZ: return Vector4u8{ 0, 1, 2, 0 };
                case SwizzleMask::XZY: return Vector4u8{ 0, 2, 1, 0 };
                case SwizzleMask::XXZ: return Vector4u8{ 0, 0, 2, 0 };
                case SwizzleMask::XZX: return Vector4u8{ 0, 2, 0, 0 };
                case SwizzleMask::XZZ: return Vector4u8{ 0, 2, 2, 0 };
                case SwizzleMask::YZX: return Vector4u8{ 1, 2, 0, 0 };
                case SwizzleMask::YZY: return Vector4u8{ 1, 2, 1, 0 };
                case SwizzleMask::YYZ: return Vector4u8{ 1, 1, 2, 0 };
                case SwizzleMask::YZZ: return Vector4u8{ 1, 2, 2, 0 };
                case SwizzleMask::ZYX: return Vector4u8{ 2, 1, 0, 0 };
                case SwizzleMask::ZXY: return Vector4u8{ 2, 0, 1, 0 };
                case SwizzleMask::ZZX: return Vector4u8{ 2, 2, 0, 0 };
                case SwizzleMask::ZZY: return Vector4u8{ 2, 2, 1, 0 };
                case SwizzleMask::ZXZ: return Vector4u8{ 2, 0, 2, 0 };
                case SwizzleMask::ZYZ: return Vector4u8{ 2, 1, 2, 0 };
                case SwizzleMask::ZZZ: return Vector4u8{ 2, 2, 2, 0 };
                case SwizzleMask::XYW: return Vector4u8{ 0, 1, 3, 0 };
                case SwizzleMask::XWZ: return Vector4u8{ 0, 3, 2, 0 };
                case SwizzleMask::XWW: return Vector4u8{ 0, 3, 3, 0 };
                case SwizzleMask::XXW: return Vector4u8{ 0, 0, 3, 0 };
                case SwizzleMask::YXW: return Vector4u8{ 1, 0, 3, 0 };
                case SwizzleMask::YZW: return Vector4u8{ 1, 2, 3, 0 };
                case SwizzleMask::YWX: return Vector4u8{ 1, 3, 0, 0 };
                case SwizzleMask::YWZ: return Vector4u8{ 1, 3, 2, 0 };
                case SwizzleMask::YYW: return Vector4u8{ 1, 1, 3, 0 };
                case SwizzleMask::ZXW: return Vector4u8{ 2, 0, 3, 0 };
                case SwizzleMask::ZYW: return Vector4u8{ 2, 1, 3, 0 };
                case SwizzleMask::ZZW: return Vector4u8{ 2, 2, 3, 0 };
                case SwizzleMask::ZWX: return Vector4u8{ 2, 3, 0, 0 };
                case SwizzleMask::ZWY: return Vector4u8{ 2, 3, 1, 0 };
                case SwizzleMask::WXY: return Vector4u8{ 3, 0, 1, 0 };
                case SwizzleMask::WXZ: return Vector4u8{ 3, 0, 2, 0 };
                case SwizzleMask::WYX: return Vector4u8{ 3, 1, 0, 0 };
                case SwizzleMask::WYZ: return Vector4u8{ 3, 1, 2, 0 };
                case SwizzleMask::WZY: return Vector4u8{ 3, 2, 1, 0 };
                case SwizzleMask::WWX: return Vector4u8{ 3, 3, 0, 0 };
                case SwizzleMask::WWY: return Vector4u8{ 3, 3, 1, 0 };
                case SwizzleMask::WWZ: return Vector4u8{ 3, 3, 2, 0 };
                case SwizzleMask::WWW: return Vector4u8{ 3, 3, 3, 0 };

                // 4D Swizzle
                case SwizzleMask::XXXX: return Vector4u8{ 0, 0, 0, 0 };
                case SwizzleMask::XXXY: return Vector4u8{ 0, 0, 0, 1 };
                case SwizzleMask::XXYY: return Vector4u8{ 0, 0, 1, 1 };
                case SwizzleMask::XYYY: return Vector4u8{ 0, 1, 1, 1 };
                case SwizzleMask::XYXY: return Vector4u8{ 0, 1, 0, 1 };
                case SwizzleMask::YYYX: return Vector4u8{ 1, 1, 1, 0 };
                case SwizzleMask::YYXX: return Vector4u8{ 1, 1, 0, 0 };
                case SwizzleMask::YXXX: return Vector4u8{ 1, 0, 0, 0 };
                case SwizzleMask::YXYX: return Vector4u8{ 1, 0, 1, 0 };
                case SwizzleMask::YYYY: return Vector4u8{ 1, 1, 1, 1 };
                case SwizzleMask::XXYZ: return Vector4u8{ 0, 0, 1, 2 };
                case SwizzleMask::XZXZ: return Vector4u8{ 0, 2, 0, 2 };
                case SwizzleMask::XXZZ: return Vector4u8{ 0, 0, 2, 2 };
                case SwizzleMask::XYZZ: return Vector4u8{ 0, 1, 2, 2 };
                case SwizzleMask::XZZZ: return Vector4u8{ 0, 2, 2, 2 };
                case SwizzleMask::YYXZ: return Vector4u8{ 1, 1, 0, 2 };
                case SwizzleMask::YZYZ: return Vector4u8{ 1, 2, 1, 2 };
                case SwizzleMask::YXZZ: return Vector4u8{ 1, 0, 2, 2 };
                case SwizzleMask::YYZX: return Vector4u8{ 1, 1, 2, 0 };
                case SwizzleMask::YYZY: return Vector4u8{ 1, 1, 2, 1 };
                case SwizzleMask::ZXYZ: return Vector4u8{ 2, 0, 1, 2 };
                case SwizzleMask::ZYYZ: return Vector4u8{ 2, 1, 1, 2 };
                case SwizzleMask::ZYXZ: return Vector4u8{ 2, 1, 0, 2 };
                case SwizzleMask::ZZXX: return Vector4u8{ 2, 2, 0, 0 };
                case SwizzleMask::ZZYY: return Vector4u8{ 2, 2, 1, 1 };
                case SwizzleMask::ZZZZ: return Vector4u8{ 2, 2, 2, 2 };
                case SwizzleMask::XYZW: return Vector4u8{ 0, 1, 2, 3 };
                case SwizzleMask::WZYX: return Vector4u8{ 3, 2, 1, 0 };
                case SwizzleMask::XXWW: return Vector4u8{ 0, 0, 3, 3 };
                case SwizzleMask::YYWW: return Vector4u8{ 1, 1, 3, 3 };
                case SwizzleMask::ZZWW: return Vector4u8{ 2, 2, 3, 3 };
                case SwizzleMask::WWWW: return Vector4u8{ 3, 3, 3, 3 };
            }

            return {};
        }

        template <typename InVectorT, typename OutVectorT>
        constexpr auto GetSwizzleMaskRange()
        {
            if constexpr (is_specialization_of_v<Vector2, OutVectorT>)
            {
                if constexpr (is_specialization_of_v<Vector2, InVectorT>) return std::pair{ SwizzleMask::XY, SwizzleMask::YY };
                if constexpr (is_specialization_of_v<Vector3, InVectorT>) return std::pair{ SwizzleMask::XY, SwizzleMask::ZZ };
                if constexpr (is_specialization_of_v<Vector4, InVectorT>) return std::pair{ SwizzleMask::XY, SwizzleMask::WW };
            }
            else if constexpr (is_specialization_of_v<Vector3, OutVectorT>)
            {
                if constexpr (is_specialization_of_v<Vector2, InVectorT>) return std::pair{ SwizzleMask::XXX, SwizzleMask::YYY };
                if constexpr (is_specialization_of_v<Vector3, InVectorT>) return std::pair{ SwizzleMask::XXX, SwizzleMask::ZZZ };
                if constexpr (is_specialization_of_v<Vector4, InVectorT>) return std::pair{ SwizzleMask::XXX, SwizzleMask::WWW };
            }
            else if constexpr (is_specialization_of_v<Vector4, OutVectorT>)
            {
                if constexpr (is_specialization_of_v<Vector2, InVectorT>) return std::pair{ SwizzleMask::XXXX, SwizzleMask::YYYY };
                if constexpr (is_specialization_of_v<Vector3, InVectorT>) return std::pair{ SwizzleMask::XXXX, SwizzleMask::ZZZZ };
                if constexpr (is_specialization_of_v<Vector4, InVectorT>) return std::pair{ SwizzleMask::XXXX, SwizzleMask::WWWW };
            }
        }

        template <typename InVectorT, typename OutVectorT>
        bool DrawSwizzleEnumPropertyGrid(SwizzleMask& currentMask)
        {
            constexpr auto startEndPair = GetSwizzleMaskRange<InVectorT, OutVectorT>();
            return Ui::PropertyGrid::DrawEnumProperty<SwizzleMask, startEndPair.first, startEndPair.second>("Swizzle", currentMask);
        }
    }

    // Float
    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f, Vector2f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f, Vector2f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f, Vector3f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f, Vector3f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f, Vector4f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f, Vector4f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f, Vector2f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f, Vector2f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f, Vector3f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f, Vector3f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f, Vector4f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f, Vector4f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f, Vector2f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f, Vector2f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f, Vector3f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f, Vector3f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f, Vector4f>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f, Vector4f>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    // Double
    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2d, Vector2d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2d, Vector2d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2d, Vector3d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2d, Vector3d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2d, Vector4d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2d, Vector4d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3d, Vector2d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3d, Vector2d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3d, Vector3d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3d, Vector3d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3d, Vector4d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3d, Vector4d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4d, Vector2d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4d, Vector2d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4d, Vector3d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4d, Vector3d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4d, Vector4d>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4d, Vector4d>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    // S32
    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2s32, Vector2s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2s32, Vector2s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2s32, Vector3s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2s32, Vector3s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2s32, Vector4s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2s32, Vector4s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3s32, Vector2s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3s32, Vector2s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3s32, Vector3s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3s32, Vector3s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3s32, Vector4s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3s32, Vector4s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4s32, Vector2s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4s32, Vector2s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4s32, Vector3s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4s32, Vector3s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4s32, Vector4s32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4s32, Vector4s32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    // S64
    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2s64, Vector2s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2s64, Vector2s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2s64, Vector3s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2s64, Vector3s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2s64, Vector4s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2s64, Vector4s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3s64, Vector2s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3s64, Vector2s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3s64, Vector3s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3s64, Vector3s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3s64, Vector4s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3s64, Vector4s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4s64, Vector2s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4s64, Vector2s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4s64, Vector3s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4s64, Vector3s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4s64, Vector4s64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4s64, Vector4s64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }
}
