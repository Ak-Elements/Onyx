#pragma once

#include <onyx/volume/source/volumebase.h>

#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>
#include <algorithm>

namespace Onyx::Volume
{
    class CSGCube : public VolumeBase
    {
    public:
        CSGCube()
            : m_Center()
            , m_HalfExtents(0.5f)
        {
        }

        CSGCube(const Vector3f& center, const Vector3f& halfExtents)
            : m_Center(center)
            , m_HalfExtents(halfExtents)
        {
        }

        bool GetUseV2() const { return m_UseV2; }
		void SetUseV2(bool val) { m_UseV2 = val; }

        virtual Vector4f GetValueAndGradient2(const Vector3f& position) const
        {
            constexpr onyxF32 difference = 0.1f;
            Vector3f gradient(GetValue(Vector3f(position[0] + difference, position[1], position[2])) - GetValue(Vector3f(position[0] - difference, position[1], position[2])),
                GetValue(Vector3f(position[0], position[1] + difference, position[2])) - GetValue(Vector3f(position[0], position[1] - difference, position[2])),
                GetValue(Vector3f(position[0], position[1], position[2] + difference)) - GetValue(Vector3f(position[0], position[1], position[2] - difference)));

            if (gradient.IsZero() == false)
            {
                gradient.Normalize();
                gradient *= -1.0f;
            }

            return Vector4f(
                gradient[0],
                gradient[1],
                gradient[2],
                GetDistanceTo2(position));
        }

        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override
        {
			if (m_UseV2)
			{
				return GetValueAndGradient2(position);
			}
			else
			{
				Vector3f gradient(GetValue(Vector3f(position[0] + 1.0f, position[1], position[2])) - GetValue(Vector3f(position[0] - 1.0f, position[1], position[2])),
					GetValue(Vector3f(position[0], position[1] + 1.0f, position[2])) - GetValue(Vector3f(position[0], position[1] - 1.0f, position[2])),
					GetValue(Vector3f(position[0], position[1], position[2] + 1.0f)) - GetValue(Vector3f(position[0], position[1], position[2] - 1.0f)));

                if (gradient.IsZero() == false)
				    gradient.Normalize();
				gradient *= -1.0f;
				return Vector4f(
					gradient[0],
					gradient[1],
					gradient[2],
					GetDistanceTo(position));
			}
        }

        virtual onyxF32 GetValue(const Vector3f& position) const override
        {
            return m_UseV2 ? GetDistanceTo2(position) : GetDistanceTo(position);
        }

        Vector3f GetCenter() const { return m_Center; }
        void SetCenter(const Vector3f& center) { m_Center = center; }

        Vector3f GetHalfExtents() const { return m_HalfExtents; }
        void SetHalfExtents (const Vector3f& halfExtents) { m_HalfExtents = halfExtents; }

    protected:
		onyxF32 GetDistanceTo2(const Vector3f& position) const
		{
			using std::abs;
			using std::max;
			using std::min;

			onyxF32 x = max(position[0] - m_Center[0] - m_HalfExtents[0],
				m_Center[0] - position[0] - m_HalfExtents[0]);

			onyxF32 y = max(position[1] - m_Center[1] - m_HalfExtents[1],
				m_Center[1] - position[1] - m_HalfExtents[1]);

			onyxF32 z = max(position[2] - m_Center[2] - m_HalfExtents[2],
				m_Center[2] - position[2] - m_HalfExtents[2]);

			onyxF32 d = x;
			d = max(d, y);
			d = max(d, z);

			return -d;
		}

        onyxF32 GetDistanceTo(const Vector3f& position) const
        {
            using std::abs;
            using std::max;
			using std::min;

            onyxF32 x = max(position[0] - m_Center[0] - m_HalfExtents[0],
                m_Center[0] - position[0] - m_HalfExtents[0]);

            onyxF32 y = max(position[1] - m_Center[1] - m_HalfExtents[1],
                m_Center[1] - position[1] - m_HalfExtents[1]);

            onyxF32 z = max(position[2] - m_Center[2] - m_HalfExtents[2],
                m_Center[2] - position[2] - m_HalfExtents[2]);

            onyxF32 d = x;
            d = max(d, y);
            d = max(d, z);
            
			Vector3f distance(x, y, z);
			return d <= 0 ? distance.Length() : -distance.Length();

            /*onyxF32 distance = 0.0f;

            const Vector3f distanceMinimum = position - m_Minimum;
            const Vector3f distanceMaximum = m_Maximum - position;

            // is inside
            if ((distanceMinimum[0] >= 0.0f) && (distanceMinimum[1] >= 0.0f) && (distanceMinimum[2] >= 0.0f) &&
                (distanceMaximum[0] >= 0.0f) && (distanceMaximum[1] >= 0.0f) && (distanceMaximum[2] >= 0.0f))
            {
                const onyxF32 distances[6] = { distanceMinimum[0], distanceMinimum[1], distanceMinimum[2],
                                            distanceMaximum[0], distanceMaximum[1], distanceMaximum[2] };
                
                distance = std::numeric_limits<onyxF32>::max();
                for (unsigned char i = 0; i < 6; ++i)
                {
                    if (distances[i] < distance)
                    {
                        distance = distances[i];
                    }
                }
            }
            else
            {
                distance = -Distance(position);
            }

            return distance;*/
        }

    protected:
        Vector3f m_Center;
        Vector3f m_HalfExtents;
		bool m_UseV2 = false;
    };
}