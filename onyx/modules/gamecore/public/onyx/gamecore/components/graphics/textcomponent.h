#pragma once
#include <onyx/gamecore/assets/sdffont.h>
#include <onyx/graphics/graphicstypes.h>
#include <onyx/graphics/vertex.h>

namespace Onyx
{namespace Graphics
    {
        class GraphicsApi;
    }

    namespace FileSystem
    {
        struct JsonValue;
    }

    class Stream;
}

namespace Onyx::GameCore
{
    struct TextComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::TextComponent";
        StringId32 GetTypeId() const { return TypeId; }

        String Text;
        Reference<SDFFont> Font;
        onyxF32 Size = 50.0;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

        void SetFont(Reference<SDFFont>& font);

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
