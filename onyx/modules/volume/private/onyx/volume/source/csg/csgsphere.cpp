#include <onyx/volume/source/csg/csgsphere.h>

namespace Onyx::Volume
{
    CSGSphere::CSGSphere()
    {

    }

    CSGSphere::CSGSphere(const onyxF32 radius, const Vector3f32& center)
        : m_Radius(radius)
        , m_Center(center)
    {

    }

    Vector4f32 CSGSphere::GetValueAndGradient(const Vector3f32& position) const
    {
        Vector3f32 gradient = position - m_Center;

        onyxF32 length = 0.0f;
        if (gradient.IsZero() == false) 
        {
            length = numeric_cast<onyxF32>(gradient.Length());
            if (length > 0.0f)
            {
                gradient = gradient * (1.0f / length);
            }
        }

        return Vector4f32(
            gradient[0],
            gradient[1],
            gradient[2],
            m_Radius - length
        );
    }

    onyxF32 CSGSphere::GetValue(const Vector3f32& position) const
    {
        const Vector3f32 pMinCenter = position - m_Center;
        return m_Radius - numeric_cast<onyxF32>(pMinCenter.Length());
    }

}