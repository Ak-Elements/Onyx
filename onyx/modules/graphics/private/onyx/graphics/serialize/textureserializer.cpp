#include <onyx/graphics/serialize/textureserializer.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/memoryaccess.h>
#include <onyx/rhi/textureproperties.h>
#include <onyx/rhi/texturestorageproperties.h>

#include <onyx/graphics/textureasset.h>
#include <onyx/filesystem/imagefile.h>

namespace Onyx::Graphics
{
    bool TextureSerializer::Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& /*engine*/) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(meta);
        ONYX_UNUSED(serializer);
        return false;
    }

    bool TextureSerializer::Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& /*deserializer*/, IEngine& engine) const
    {
        GraphicsSystem& graphicsSystem = engine.GetSystem<GraphicsSystem>();
        TextureAsset& textureAsset = asset.As<TextureAsset>();

        FileSystem::ImageFile file(FileSystem::Path::ReplaceExtension(meta.Path, "png"));

        TextureStorageProperties storageProps;
        storageProps.m_Format = /*(file.GetChannelCount() == 4) ? */TextureFormat::RGBA_UNORM8;// : Graphics::TextureFormat::RGB_UNORM8;
        storageProps.m_Type = TextureType::Texture2D;
        storageProps.m_Size = { file.GetSize()[0], file.GetSize()[1], 1 };
        storageProps.m_MaxMipLevel = 1;
        storageProps.m_ArraySize = 0;
        storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality 
        storageProps.m_CpuAccess = CPUAccess::None;
        storageProps.m_GpuAccess = GPUAccess::Read;
        storageProps.m_IsTexture = true;
        storageProps.m_DebugName = Format::Format("{} Texture Storage", meta.GetName());

        TextureProperties textureProps;
        textureProps.m_Format = storageProps.m_Format;
        textureProps.m_DebugName = Format::Format("{} Texture", meta.GetName());
        const Span<onyxU8>& imageData = file.GetData();

        graphicsSystem.CreateTexture(textureAsset.m_Texture, storageProps, textureProps, imageData);
        return true;

        //TextureAsset& textureAsset = asset.As<TextureAsset>();

        //onyxU8 channelCount = 0;
        //serializer.Read(channelCount);

        //Vector2s32 size;
        //serializer.Read(size);

        //DynamicArray<onyxU8> imageData;
        //serializer.Read(imageData);

        //Graphics::TextureStorageProperties storageProps;
        //storageProps.m_Format = (channelCount == 4) ? Graphics::TextureFormat::RGBA_UNORM8 : Graphics::TextureFormat::RGB_UNORM8;
        //storageProps.m_Type = Graphics::TextureType::Texture2D;
        //storageProps.m_Size = { size[0], size[1], 1 };
        //storageProps.m_MaxMipLevel = 1;
        //storageProps.m_ArraySize = 0;
        //storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality 
        //storageProps.m_CpuAccess = Graphics::CPUAccess::None;
        //storageProps.m_GpuAccess = Graphics::GPUAccess::Read;
        //storageProps.m_IsTexture = true;

        //Graphics::TextureProperties textureProps;
        //textureProps.m_Format = storageProps.m_Format;

        //Span<onyxU8> imageDataSpan { imageData.data(),  imageData.size() };
        //m_GraphicsSystem->CreateTexture(textureAsset.m_Texture, storageProps, textureProps, imageDataSpan);

        return true;
    }
}
