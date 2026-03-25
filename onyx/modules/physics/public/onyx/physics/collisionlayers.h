#pragma once

namespace onyx::physics {
enum class CollisionLayer : uint16_t {
    STATIC,
    DYNAMIC,
    COUNT,
};
}