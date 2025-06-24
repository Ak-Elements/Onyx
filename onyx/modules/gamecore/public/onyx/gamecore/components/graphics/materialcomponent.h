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

namespace Onyx
{

namespace GameCore
{
    struct MaterialComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::MaterialComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Assets::AssetId MaterialId;
        Reference<Graphics::MaterialShaderGraph> Material;

        void LoadMaterial(Assets::AssetSystem& assetSystem);
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawImGuiEditor();
#endif
    };
}

template <>
struct Serialization<GameCore::MaterialComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::MaterialComponent& material);
    static bool Deserialize(const Deserializer& deserializer, GameCore::MaterialComponent& outMaterial);
};

}
