#pragma once

namespace onyx::rhi {

enum class ProjectionType : uint8_t {
    Perspective,
    Orthographic,
};

struct CameraSettings {
    ProjectionType Type = ProjectionType::Perspective;

    units::RadiansF32 FieldOfView = units::RadiansF32( units::DegreesF32( 45.0 ) );
    float32 OrthographicSize = 1.0f;

    float32 NearPlane = 0.1f;
    float32 FarPlane = 65536.0f;
};
} // namespace onyx::rhi
