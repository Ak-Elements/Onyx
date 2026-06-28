#pragma once

namespace onyx::physics {
namespace jolt {
class SdfShape;
}

class SdfShape {
  public:
    using SdfFunction = Vector4f32 ( * )( Vector3f32 );

    SdfShape( SdfFunction function );

  private:
    UniquePtr< jolt::SdfShape > m_shape;
};
} // namespace onyx::physics
