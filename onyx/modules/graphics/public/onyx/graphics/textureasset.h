#pragma once
#include <onyx/assets/asset.h>

#include <onyx/rhi/graphicshandles.h>

namespace onyx::graphics
{
    class TextureAsset : public assets::Asset<TextureAsset>
    {
    public:
        static constexpr StringId32 TypeId{ "onyx::graphics::assets::TextureAsset" };
        StringId32 GetTypeId() const { return TypeId; }

        const rhi::TextureHandle& GetTextureHandle() const { return m_Texture; }

    private:
        friend struct TextureSerializer;
        void SetTexture(const rhi::TextureHandle& handle);

    private:
        rhi::TextureHandle m_Texture;
    };
}
