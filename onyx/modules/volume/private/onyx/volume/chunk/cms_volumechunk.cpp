#include <onyx/volume/chunk/cms_volumechunk.h>

namespace onyx::volume
{
    namespace CubicalMarchingSquares
    {
        void VolumeChunk_CMS::SetHermiteData(onyxU64 index, const Vector4f32& gradient)
        {
            //onyxAssert(index < m_VolumeData.size());
            m_VolumeData[index].Gradient = gradient;
        }

        const HermiteData& VolumeChunk_CMS::GetHermiteData(onyxU64 index) const
        {
            //onyxAssert(index < m_VolumeData.size());
            return m_VolumeData[index];
        }
    }
}



