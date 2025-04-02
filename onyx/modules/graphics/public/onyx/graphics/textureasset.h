#pragma once
#include <onyx/assets/asset.h>

#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
{
    class TextureAsset : public Assets::Asset<TextureAsset>
    {
    public:
        const TextureHandle& GetTextureHandle() const { return m_Texture; }

    private:
        friend struct TextureSerializer;
        void SetTexture(const TextureHandle& handle);

    private:
        TextureHandle m_Texture;
    };
}
