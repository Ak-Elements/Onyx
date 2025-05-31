#pragma once
#include <onyx/graphics/font/sdffont.h>
#include <onyx/graphics/graphicstypes.h>
#include <onyx/graphics/vertex.h>

namespace Onyx
{

namespace GameCore
{
    struct TextComponent
    {
        friend struct Serialization<TextComponent>;

    public:
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::TextComponent";
        StringId32 GetTypeId() const { return TypeId; }

        String Text;
        Assets::AssetId FontId;
        Reference<Graphics::SDFFont> Font;
        onyxF32 Size = 50.0;

        void SetFont(Reference<Graphics::SDFFont>& font);

        const DynamicArray<Graphics::FontVertex>& GetVertices() const { return m_Vertices; }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif

    private:
        onyxF32 GetTextWidth(const String_U32& text);
        void UpdateVertexBuffers();

    private:
        DynamicArray<Graphics::FontVertex> m_Vertices;
    };
}

template <>
struct Serialization<GameCore::TextComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::TextComponent& textComponent);
    static bool Deserialize(const Deserializer& deserializer, GameCore::TextComponent& outTextComponent);
};

}
