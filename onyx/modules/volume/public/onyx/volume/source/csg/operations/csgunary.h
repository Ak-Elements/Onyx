#pragma once

#include <onyx/volume/source/volumebase.h>

namespace Onyx::Volume
{
    class CSGUnary : public VolumeBase
    {
    public:
        virtual const VolumeBase* GetVolume() const { return m_Volume; }
        virtual void SetVolume(const VolumeBase* volume) { m_Volume = volume; }
    
    protected:
        CSGUnary(const VolumeBase* /*volume*/) {}
        CSGUnary() {}

    protected:
        const VolumeBase* m_Volume = nullptr;
    };
}