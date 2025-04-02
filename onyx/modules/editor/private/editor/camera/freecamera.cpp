//#include <editor/camera/freecamera.h>
//#include <onyx/graphics/window/windows/nativewindow.h>
//
//#include <imgui_internal.h>
//
//namespace Onyx::Application
//{
//    void FreeCamera::Init()
//    {
//        m_Direction = -Vector3f::Z_Unit();
//        m_RightDirection = Vector3f::X_Unit();
//        m_UpDirection = Vector3f::Y_Unit();
//
//        m_Orientation = Rotor3f(0.0f, Bivector3f32::ZX_Unit());
//    }
//
//    void FreeCamera::Update(onyxU64 deltaTime)
//    {
//        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
//        {
//            constexpr onyxF32 MAX_ROTATION_SPEED = 0.12f;
//
//            onyxF32 dt = static_cast<onyxF32>(deltaTime);
//
//            const onyxF32 yawSign = m_UpDirection[1] < 0 ? -1.0f : 1.0f;
//            const Vector3f globalUp(0.0f, yawSign, 0.0f);
//
//            // y direction is up / down (forward / back)
//            m_Position += m_RightDirection * m_InputDirection[0] * dt * m_Speed;
//            m_Position += globalUp * m_InputDirection[1] * dt * m_Speed;
//            m_Position += m_Direction * m_InputDirection[2] * dt * m_Speed;
//
//            m_YawDelta += std::clamp(yawSign * m_InputRotation[0] * m_RotationSpeed, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
//            m_PitchDelta += std::clamp(m_InputRotation[1] * m_RotationSpeed, -MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
//        }
//
//        m_Yaw += m_YawDelta;
//        m_Pitch += m_PitchDelta;
//
//        m_YawDelta *= 0.6f;
//        m_PitchDelta *= 0.6f;
//
//        if ((IsZero(m_YawDelta) == false) ||
//            (IsZero(m_PitchDelta) == false))
//        {
//            Rotor3f pitchRotor(-m_Pitch, Bivector3f32::YZ_Unit());
//            Rotor3f yawRotor(-m_Yaw, Bivector3f32::ZX_Unit());
//
//            m_Orientation = (yawRotor * pitchRotor).Normalized();
//
//            m_Direction = m_Orientation.rotate(-Vector3f::Z_Unit());
//            m_RightDirection = m_Orientation.rotate(Vector3f::X_Unit());
//            m_UpDirection = m_Orientation.rotate(Vector3f::Y_Unit());
//        }
//
//        //// update view matrix
//        LookAt(m_Position, m_Position + GetForward(), GetUp());
//    }
//
//    void FreeCamera::SetPosition(const Vector3f& position)
//    {
//        m_Position = position;
//
//        Matrix4<onyxF32> transformMatrix;
//        transformMatrix[3] = (transformMatrix[0] * m_Position[0]) + (transformMatrix[1] * m_Position[1]) + (transformMatrix[2] * m_Position[2]) + transformMatrix[3];
//
//        Matrix4<onyxF32> rotationMatrix = GetOrientation().ToMatrix4();
//
//        m_ViewMatrix = transformMatrix * rotationMatrix;
//        m_InverseViewMatrix = m_ViewMatrix.Inverse();
//    }
//
//    Vector3f FreeCamera::GetForward() const
//    {
//        return m_Direction;
//    }
//
//    Vector3f FreeCamera::GetUp() const
//    {
//        return m_UpDirection;
//    }
//
//    Vector3f FreeCamera::GetRight() const
//    {
//        return m_RightDirection;
//    }
//
//    Rotor3f FreeCamera::GetOrientation() const
//    {
//        return m_Orientation;
//    }
//}
