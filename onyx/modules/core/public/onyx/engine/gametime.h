#pragma once
#include <onyx/noncopyable.h>

namespace Onyx
{
    struct GameTime
    {
        GameTime(onyxU64 milliseconds)
            : m_Milliseconds(milliseconds)
        {
        }

        operator onyxU64() const { return m_Milliseconds; }

        GameTime operator+(GameTime other) const { return m_Milliseconds + other.m_Milliseconds; }
        GameTime operator-(GameTime other) const { return m_Milliseconds - other.m_Milliseconds; }

        onyxU64 GetMilliseconds() const { return m_Milliseconds; }
    private:
        onyxU64 m_Milliseconds;
    };
}