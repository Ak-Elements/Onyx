#pragma once

namespace onyx {
struct DeltaGameTime {
    DeltaGameTime( uint64_t milliseconds )
        : DeltaMilliseconds( milliseconds ) {}

    DeltaGameTime operator+( DeltaGameTime other ) const { return { DeltaMilliseconds + other.DeltaMilliseconds }; }
    DeltaGameTime operator-( DeltaGameTime other ) const { return { DeltaMilliseconds - other.DeltaMilliseconds }; }

    uint64_t DeltaMilliseconds;
};

struct GameTime {
    GameTime( uint64_t milliseconds )
        : Milliseconds( milliseconds ) {}

    GameTime operator+( GameTime other ) const { return { Milliseconds + other.Milliseconds }; }
    GameTime operator-( GameTime other ) const { return { Milliseconds - other.Milliseconds }; }

    uint64_t Milliseconds;
};
} // namespace onyx