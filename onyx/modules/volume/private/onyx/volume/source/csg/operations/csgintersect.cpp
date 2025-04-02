#include <onyx/volume/source/csg/operations/csgintersect.h>

namespace Onyx::Volume
{
    CSGIntersect::CSGIntersect()
    {
    }

    CSGIntersect::CSGIntersect(VolumeBase* first, VolumeBase* second)
        : CSGOperation(first, second)
    {

    }

    Vector4f CSGIntersect::GetValueAndGradient(const Vector3f& position) const
    {
        if (m_First && m_Second)
        {
            Vector4f valueA = m_First->GetValueAndGradient(position);
            Vector4f valueB = m_Second->GetValueAndGradient(position);
            if (valueA[3] < valueB[3])
            {
                return valueA;
            }
            return valueB;
        }

        return Vector4f(std::numeric_limits<onyxF32>::max());
    }

    onyxF32 CSGIntersect::GetValue(const Vector3f& position) const
    {
        if (m_First && m_Second)
        {
            onyxF32 valueA = m_First->GetValue(position);
            onyxF32 valueB = m_Second->GetValue(position);
            if (valueA < valueB)
            {
                return valueA;
            }
            return valueB;
        }

        return std::numeric_limits<onyxF32>::max();
    }

}