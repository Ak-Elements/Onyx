#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx
{
    namespace Graphics
    {
        class MaterialShaderGraph;
    }

    class Stream;

    namespace Assets
    {
        struct AssetId;
    }

    namespace FileSystem
    {
        struct JsonValue;
    }

}

namespace Onyx::GameCore
{
    struct MaterialComponent
    {
        Assets::AssetId MaterialId;
        Reference<Graphics::MaterialShaderGraph> Material;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

        void LoadMaterial(Assets::AssetSystem& assetSystem);
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };
}
