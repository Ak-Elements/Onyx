#include <onyx/gamecore/components/graphics/materialcomponent.h>
#include <onyx/filesystem/onyxfile.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::GameCore
{
    void MaterialComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void MaterialComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void MaterialComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        outStream.Set("material", MaterialId.Get());
    }

    void MaterialComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        String path;
        onyxU64 materialId;
        if (inStream.Get("material", materialId))
        {
            MaterialId = materialId;
        }
    }
}
