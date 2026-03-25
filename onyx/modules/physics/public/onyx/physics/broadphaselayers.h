#pragma once

namespace onyx::physics {
enum class BroadPhaseLayer : uint8_t {
    STATIC,
    DYNAMIC,
    COUNT,
};
}