#pragma once

#include <onyx/volume/source/volumebase.h>

#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

namespace Onyx::Volume
{
    class CSGPlane : public VolumeBase
    {
    public:
        CSGPlane() = default;
        CSGPlane(const onyxF32 distance, const Vector3f& normal)
            : m_Distance(distance)
            , m_Normal(normal)
        {
        }

        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override
        {
            return Vector4f(
                m_Normal[0],
                m_Normal[1],
                m_Normal[2],
                m_Distance - static_cast<onyxF32>(m_Normal.Dot(position)));
        }

        virtual onyxF32 GetValue(const Vector3f& position) const override
        {
            return m_Distance - static_cast<onyxF32>(m_Normal.Dot(position));
        }

        onyxF32 GetDistance() const { return m_Distance; }
		void SetDistance(onyxF32 distance) { m_Distance = distance; }

        const Vector3f& GetNormal() const { return m_Normal; }
		void SetNormal(const Vector3f& normal) { m_Normal = normal; }

    protected:
        onyxF32 m_Distance = 0.0f;
        Vector3f m_Normal = Vector3f::Y_Unit();
    };
}