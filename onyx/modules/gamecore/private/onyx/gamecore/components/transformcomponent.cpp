#include <onyx/gamecore/components/transformcomponent.h>

#include <onyx/filesystem/onyxfile.h>

namespace Onyx::GameCore
{
    void TransformComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void TransformComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void TransformComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        std::array<onyxF32, 3> translation { m_Translation[0], m_Translation[1], m_Translation[2] };
        outStream.Set("translation", translation);

        std::array<onyxF32, 3> rotation { m_RotationEuler[0], m_RotationEuler[1], m_RotationEuler[2] };
        outStream.Set("rotation", rotation);

        std::array<onyxF32, 3> scale { m_Scale[0], m_Scale[1], m_Scale[2] };
        outStream.Set("scale", scale);
    }

    void TransformComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        std::array<onyxF32, 3> translation{ 0.0f };
        if (inStream.Get("translation", translation))
        {
            m_Translation[0] = translation[0];
            m_Translation[1] = translation[1];
            m_Translation[2] = translation[2];
        }

        std::array<onyxF32, 3> rotation{ 0.0f };
        if (inStream.Get("rotation", rotation))
        {
            SetRotation({ rotation[0], rotation[1], rotation[2] });
        }

        std::array<onyxF32, 3> scale{ 0.0f };
        if (inStream.Get("scale", scale))
        {
            m_Scale[0] = scale[0];
            m_Scale[1] = scale[1];
            m_Scale[2] = scale[2];
        }
    }
}
