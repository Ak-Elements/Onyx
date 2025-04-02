#include <onyx/volume/source/csg/operations/csgdifference.h>

namespace Onyx::Volume
{
    CSGDifference::CSGDifference()
    {
    }

    CSGDifference::CSGDifference(VolumeBase* first, VolumeBase* second) : CSGOperation(first, second)
    {
    }

    Vector4f CSGDifference::GetValueAndGradient(const Vector3f& position) const
    {
        if (m_First && m_Second)
        {
            Vector4f valueA = m_First->GetValueAndGradient(position);
            Vector4f valueB = -1.0f * m_Second->GetValueAndGradient(position);
            if (valueA[3] < valueB[3])
            {
                return valueA;
            }
            return valueB;
        }

        return Vector4f(std::numeric_limits<onyxF32>::max());
    }

    onyxF32 CSGDifference::GetValue(const Vector3f& position) const
    {
        if (m_First && m_Second)
        {
            onyxF32 valueA = m_First->GetValue(position);
            onyxF32 valueB = -1.0f * m_Second->GetValue(position);
            if (valueA < valueB)
            {
                return valueA;
            }
            return valueB;
        }

        return std::numeric_limits<onyxF32>::max();
    }

}