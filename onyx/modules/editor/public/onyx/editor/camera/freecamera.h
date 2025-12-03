//#pragma once
//
//#include <onyx/geometry/rotor3.h>
//#include <onyx/graphics/camera.h>
//
//namespace Onyx::Graphics
//{
//    class Window;
//}
//
//namespace Onyx::Application
//{
//    struct InputActionContext;
//
//    class FreeCamera : public Graphics::Camera
//    {
//    public:
//        void Init();
//
//        void Update(onyxU64 deltaTime);
//
//        const Vector3f& GetPosition() const { return m_Position; }
//        void SetPosition(const Vector3f& position);
//
//        void SetInputDirection(const Vector3f& direction) { m_InputDirection = direction; }
//        void SetInputRotation(const Vector3f& rotation) { m_InputRotation = rotation * 0.002f; }
//
//        float GetPitch() const { return m_Pitch; }
//        float GetPitchDelta() const { return m_PitchDelta; }
//
//        float GetYaw() const { return m_Yaw; }
//        float GetYawDelta() const { return m_YawDelta; }
//
//        onyxF32 GetVelocity() const { return m_Speed; }
//        void SetVelocity(onyxF32 velocity) { m_Speed = std::clamp(velocity, m_MinSpeed, m_MaxSpeed); }
//
//        onyxF32 GetBaseVelocity() const { return m_BaseSpeed; }
//        onyxF32 GetVelocityIncrementFactor() const { return m_SpeedIncrementFactor; }
//
//    private:
//        Vector3f GetForward() const;
//        Vector3f GetUp() const;
//        Vector3f GetRight() const;
//        Rotor3f GetOrientation() const;
//
//    private:
//        // should be moved to some sort of settings
//        onyxF32 m_BaseSpeed = 0.002f;
//        onyxF32 m_MinSpeed = 0.001f;
//        onyxF32 m_MaxSpeed = 0.3f;
//
//        onyxF32 m_RotationSpeed = 0.3f;
//        onyxF32 m_SpeedIncrementFactor = 0.3f;
//        //
//
//        onyxF32 m_Speed = m_BaseSpeed;
//
//        Vector3f m_Position;
//        Vector3f m_RightDirection;
//        Vector3f m_UpDirection;
//
//        // rotation around X
//        onyxF32 m_Pitch = 0.0f;
//        // rotation around Y
//        onyxF32 m_Yaw = 0.0f;
//
//        Vector3f m_InputDirection;
//        Vector3f m_InputRotation;
//
//        onyxF32 m_PitchDelta = 0.0f;
//        onyxF32 m_YawDelta = 0.0f;
//
//        Rotor3f m_Orientation;
//    };
//}
