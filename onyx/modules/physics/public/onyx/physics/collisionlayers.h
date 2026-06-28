#pragma once

namespace onyx::physics {
enum class CollisionLayer : uint16_t {
    Static,
    Dynamic,
    Player,
    Count,
};
}
