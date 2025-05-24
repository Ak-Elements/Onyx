#pragma once
#include <onyx/noncopyable.h>

namespace Onyx
{
    struct DeltaGameTime
    {
        DeltaGameTime(onyxU64 milliseconds)
            : DeltaMilliseconds(milliseconds)
        {
        }

        DeltaGameTime operator+(DeltaGameTime other) const { return DeltaMilliseconds + other.DeltaMilliseconds; }
        DeltaGameTime operator-(DeltaGameTime other) const { return DeltaMilliseconds - other.DeltaMilliseconds; }

        onyxU64 DeltaMilliseconds;
    };

    struct GameTime
    {
        GameTime(onyxU64 milliseconds)
            : Milliseconds(milliseconds)
        {
        }

        GameTime operator+(GameTime other) const { return Milliseconds + other.Milliseconds; }
        GameTime operator-(GameTime other) const { return Milliseconds - other.Milliseconds; }

        onyxU64 Milliseconds;
    };
}