#pragma once

#include <onyx/eventhandler.h>
//#include <onyx/volume/mesh/meshbuilder.h>

namespace Onyx::Volume
{
    class MeshBuilder;
    namespace CubicalMarchingSquares
    {
        struct HermiteData
        {
            Vector4f Gradient;
        };

        class VolumeChunk_CMS
        {
        public:
            VolumeChunk_CMS() = default;
            VolumeChunk_CMS(onyxU32 size)
                : m_VolumeData()
            {
                m_VolumeData.resize(size);
            }

            const Vector3f& GetPosition() const { return m_Position; }
            void SetPosition(const Vector3f& position) { m_Position = position; }

            void SetSize(onyxU32 size) { m_VolumeData.resize(size); }

            void SetHermiteData(onyxU64 index, const Vector4f& gradient);
            const HermiteData& GetHermiteData(onyxU64 index) const;

            ONYX_EVENT(MeshChanged, const MeshBuilder&);

        private:
            Vector3f m_Position{};
            std::vector<HermiteData> m_VolumeData; // z ordered data

        };
    }
}
