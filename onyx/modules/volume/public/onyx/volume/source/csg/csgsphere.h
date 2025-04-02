#pragma once

#include <onyx/volume/source/volumebase.h>

namespace Onyx::Volume
{
    class CSGSphere : public VolumeBase
    {
    public:
        CSGSphere();
        CSGSphere(const onyxF32 radius, const Vector3f& center);

        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override;

        virtual onyxF32 GetValue(const Vector3f& position) const override;

        void SetRadius(onyxF32 r) { m_Radius = r; }
        void SetCenter(const Vector3f& position) { m_Center = position; }

        const Vector3f& GetCenter() const { return m_Center; }
        onyxF32 GetRadius() const { return m_Radius; }

    protected:
        onyxF32 m_Radius = 0.0f;
        Vector3f m_Center = { 0.0f, 0.0f, 0.0f };
    };
}

