#pragma once

#include <onyx/volume/source/volumebase.h>

namespace Onyx::Volume
{
    class CSGOperation : public VolumeBase
    {
    public:
        const VolumeBase* GetFirst() const { return m_First; }
        void SetFirst(VolumeBase* first) { m_First = first; }
        const VolumeBase* GetSecond() const { return m_Second; }
        void SetSecond(VolumeBase* second) { m_Second = second; }

    protected:
        CSGOperation(VolumeBase* first, VolumeBase* second)
            : m_First(first)
            , m_Second(second)
        {

        }

        CSGOperation() {}

    protected:
        VolumeBase* m_First = nullptr;
        VolumeBase* m_Second = nullptr;
    };
}