#pragma once
#include <onyx/assets/asset.h>

#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
{
    class TextureAsset : public Assets::Asset<TextureAsset>
    {
    public:
        static constexpr StringId32 TypeId{ "Onyx::Graphics::Assets::TextureAsset" };
        StringId32 GetTypeId() const { return TypeId; }

        const TextureHandle& GetTextureHandle() const { return m_Texture; }

    private:
        friend struct TextureSerializer;
        void SetTexture(const TextureHandle& handle);

    private:
        TextureHandle m_Texture;
    };
}
