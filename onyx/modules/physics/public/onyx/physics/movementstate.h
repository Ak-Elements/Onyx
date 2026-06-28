#pragma once

namespace onyx::physics {
enum class MovementState : uint8_t {
    Invalid,
    OnGround,
    Sliding,
    Falling,
    InAir,
};
}
