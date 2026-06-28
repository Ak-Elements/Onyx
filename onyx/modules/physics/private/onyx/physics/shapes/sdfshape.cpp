#include <onyx/physics/shapes/sdfshape.h>

#include <onyx/physics/jolt/joltsdfshape.h>
namespace onyx::physics {

SdfShape::SdfShape( SdfFunction function )
    : m_shape( makeUnique< jolt::SdfShape >( function ) ) {}
} // namespace onyx::physics
