#include <onyx/graphics/shadergraph/nodes/math/vectornodes.h>
#include <onyx/ui/propertygrid.h>

namespace onyx::editor
{
    namespace 
    {
        template <typename InVectorT, typename OutVectorT>
        constexpr auto GetSwizzleMaskRange()
        {
            if constexpr (IsVector2<OutVectorT>)
            {
                if constexpr (IsVector2<InVectorT>) return std::pair{ SwizzleMask::XY, SwizzleMask::YY };
                if constexpr (IsVector3<InVectorT>) return std::pair{ SwizzleMask::XY, SwizzleMask::ZZ };
                if constexpr (IsVector4<InVectorT>) return std::pair{ SwizzleMask::XY, SwizzleMask::WW };
            }
            else if constexpr (IsVector3<OutVectorT>)
            {
                if constexpr (IsVector2<InVectorT>) return std::pair{ SwizzleMask::XXX, SwizzleMask::YYY };
                if constexpr (IsVector3<InVectorT>) return std::pair{ SwizzleMask::XXX, SwizzleMask::ZZZ };
                if constexpr (IsVector4<InVectorT>) return std::pair{ SwizzleMask::XXX, SwizzleMask::WWW };
            }
            else if constexpr (IsVector4<OutVectorT>)
            {
                if constexpr (IsVector2<InVectorT>) return std::pair{ SwizzleMask::XXXX, SwizzleMask::YYYY };
                if constexpr (IsVector3<InVectorT>) return std::pair{ SwizzleMask::XXXX, SwizzleMask::ZZZZ };
                if constexpr (IsVector4<InVectorT>) return std::pair{ SwizzleMask::XXXX, SwizzleMask::WWWW };
            }
        }

        template <typename InVectorT, typename OutVectorT>
        bool DrawSwizzleEnumPropertyGrid(SwizzleMask& currentMask)
        {
            constexpr auto startEndPair = GetSwizzleMaskRange<InVectorT, OutVectorT>();
            return ui::property_grid::DrawEnumPropertyFromTo<SwizzleMask, startEndPair.first, startEndPair.second>("Swizzle", currentMask);
        }
    }

    // Float
    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f32, Vector2f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f32, Vector2f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f32, Vector3f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f32, Vector3f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f32, Vector4f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f32, Vector4f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f32, Vector2f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f32, Vector2f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f32, Vector3f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f32, Vector3f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f32, Vector4f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f32, Vector4f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f32, Vector2f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f32, Vector2f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f32, Vector3f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f32, Vector3f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f32, Vector4f32>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f32, Vector4f32>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    // Double
    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f64, Vector2f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f64, Vector2f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f64, Vector3f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f64, Vector3f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector2f64, Vector4f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector2f64, Vector4f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f64, Vector2f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f64, Vector2f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f64, Vector3f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f64, Vector3f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector3f64, Vector4f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector3f64, Vector4f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f64, Vector2f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f64, Vector2f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f64, Vector3f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f64, Vector3f64>(currentMask))
        {
            return ConvertMaskToVector(currentMask);
        }

        return {};
    }

    Optional<Vector4u8> EditorSwizzleVectorControl<Vector4f64, Vector4f64>::Draw(SwizzleMask& currentMask)
    {
        if (DrawSwizzleEnumPropertyGrid<Vector4f64, Vector4f64>(currentMask))
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
