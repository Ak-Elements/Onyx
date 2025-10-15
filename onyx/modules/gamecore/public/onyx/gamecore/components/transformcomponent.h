#pragma once

#include <onyx/geometry/common.h>
#include <onyx/geometry/rotor3.h>

namespace Onyx::GameCore
{
    struct TransformComponent
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::TransformComponent";
        StringId32 GetTypeId() const { return TypeId; }

        TransformComponent()
        {
            m_Rotation.FromEulerAngles(0, 0, 0);
        }

        TransformComponent(const Vector3f32& position)
            : m_Translation(position)
        {
            m_Rotation.FromEulerAngles(0, 0, 0);
        }

        const Vector3f32& GetTranslation() const { return m_Translation; }
        void SetTranslation(const Vector3f32& translation) { m_Translation = translation; }

        const Rotor3f32 GetRotation() const { return m_Rotation; }
        const Vector3f32& GetRotationEuler() const { return m_RotationEuler; }

        const Vector3f32& GetScale() const { return m_Scale; }
        void SetScale(const Vector3f32& scale) { m_Scale = scale; }

        void SetRotation(const Rotor3f32& rotation)
        {
            if (m_Rotation == rotation)
            {
                return;
            }

            m_Rotation = rotation;
            m_RotationEuler = m_Rotation.ToEulerAngles();
        }

        void SetRotation(const Vector3f32& eulerAngles)
        {
            if (m_RotationEuler == eulerAngles)
                return;

            m_RotationEuler = eulerAngles;
            m_Rotation = Rotor3f32::FromEulerAngles(m_RotationEuler);
        }

        void Rotate(const Vector3f32& eulerAngles)
        {
            const Vector3f32& newEuler = m_RotationEuler + eulerAngles;
            SetRotation(newEuler);
        }

        Matrix3<onyxF32> GetRotationMatrix() const
        {
            return m_Rotation.ToMatrix3();
        }

        Matrix3<onyxF32> GetScaleMatrix() const
        {
            Vector3<onyxF32> column0{ 1, 0, 0 };
            Vector3<onyxF32> column1{ 0, 1, 0 };
            Vector3<onyxF32> column2{ 0, 0, 1 };

            return Matrix3{ column0 * m_Scale[0], column1 * m_Scale[1], column2 * m_Scale[2] };
        }

        Matrix4<onyxF32> GetTransform() const
        {
            // Translation matrix from Translation
            // * Rotation (quaternion / rotor3d)
            // * Scale matrix

            Matrix4<onyxF32> transformMatrix;
            transformMatrix[3] = transformMatrix[0] * m_Translation[0] + transformMatrix[1] * m_Translation[1] + transformMatrix[2] * m_Translation[2] + transformMatrix[3];

            Matrix3<onyxF32> scaleMatrix = GetScaleMatrix();
            Matrix4<onyxF32> scaleMatrix4d
            {
                Vector4(scaleMatrix[0], 0.0f), Vector4(scaleMatrix[1], 0.0f), Vector4(scaleMatrix[2], 0.0f), Vector4(0.0f,0.0f,0.0f,1.0f)
            };

            return transformMatrix * m_Rotation.ToMatrix4() * scaleMatrix4d;
        }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawImGuiEditor();
#endif

    private:
        Vector3f32 m_Translation;
        Vector3f32 m_Scale{ 1.0f };

        Vector3f32 m_RotationEuler;
        Rotor3f32 m_Rotation;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<GameCore::TransformComponent>
    {
        static bool Serialize(Serializer& serializer, const GameCore::TransformComponent& transform);
        static bool Deserialize(const Deserializer& deserializer, GameCore::TransformComponent& outTransform);
    };
}
