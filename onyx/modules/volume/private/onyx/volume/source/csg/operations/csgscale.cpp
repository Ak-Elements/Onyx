#include <onyx/volume/source/csg/operations/csgscale.h>

namespace Onyx::Volume
{
    CSGScale::CSGScale()
        : m_Scale(1.0f)
    {
    }

    CSGScale::CSGScale(const VolumeBase* volume, const onyxF32 scale)
        : CSGUnary(volume)
        , m_Scale(scale)
    {
    }

    onyxF32 CSGScale::GetValue(const Vector3f32& /*position*/) const
    {
        return m_Scale;
    }

    Vector4f32 CSGScale::GetValueAndGradient(const Vector3f32& /*position*/) const
    {
        return Vector4f32();
    }
}