#include <onyx/graphics/serialize/textureserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/graphicsapi.h>

#include <onyx/graphics/textureasset.h>
#include <onyx/graphics/textureproperties.h>
#include <onyx/filesystem/imagefile.h>

namespace Onyx::Graphics
{
    TextureSerializer::TextureSerializer(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi)
        : AssetSerializer(assetSystem)
        , m_GraphicsApi(&graphicsApi)
    {
    }

    bool TextureSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool TextureSerializer::SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(filePath);
        return true;
    }

    bool TextureSerializer::SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool TextureSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        if (m_GraphicsApi == nullptr)
            return false;

        TextureAsset& textureAsset = asset.As<TextureAsset>();

        onyxU8 channelCount = 0;
        inStream.ReadRaw(channelCount);

        Vector2s32 size;
        inStream.ReadRaw(size);

        DynamicArray<onyxU8> imageData;
        inStream.ReadRaw(imageData);

        Graphics::TextureStorageProperties storageProps;
        storageProps.m_Format = (channelCount == 4) ? Graphics::TextureFormat::RGBA_UNORM8 : Graphics::TextureFormat::RGB_UNORM8;
        storageProps.m_Type = Graphics::TextureType::Texture2D;
        storageProps.m_Size = { size[0], size[1], 1 };
        storageProps.m_MaxMipLevel = 1;
        storageProps.m_ArraySize = 0;
        storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality 
        storageProps.m_CpuAccess = Graphics::CPUAccess::None;
        storageProps.m_GpuAccess = Graphics::GPUAccess::Read;
        storageProps.m_IsTexture = true;

        Graphics::TextureProperties textureProps;
        textureProps.m_Format = storageProps.m_Format;

        Span<onyxU8> imageDataSpan { imageData.data(),  imageData.size() };
        m_GraphicsApi->CreateTexture(textureAsset.m_Texture, storageProps, textureProps, imageDataSpan);

        return true;
    }

    bool TextureSerializer::DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        //FileSystem::OnyxFile file(FileSystem::Path::ReplaceExtension(filePath, "png"));
        //FileSystem::FileStream stream = file.OpenStream(FileSystem::OpenMode::Read | FileSystem::OpenMode::Binary);

        TextureAsset& textureAsset = asset.As<TextureAsset>();

        FileSystem::ImageFile file(FileSystem::Path::ReplaceExtension(filePath, "png"));

        Graphics::TextureStorageProperties storageProps;
        storageProps.m_Format = /*(file.GetChannelCount() == 4) ? */Graphics::TextureFormat::RGBA_UNORM8;// : Graphics::TextureFormat::RGB_UNORM8;
        storageProps.m_Type = Graphics::TextureType::Texture2D;
        storageProps.m_Size = { file.GetSize()[0], file.GetSize()[1], 1 };
        storageProps.m_MaxMipLevel = 1;
        storageProps.m_ArraySize = 0;
        storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality 
        storageProps.m_CpuAccess = Graphics::CPUAccess::None;
        storageProps.m_GpuAccess = Graphics::GPUAccess::Read;
        storageProps.m_IsTexture = true;
        storageProps.m_DebugName = Format::Format("{} Texture Storage", filePath.stem());

        Graphics::TextureProperties textureProps;
        textureProps.m_Format = storageProps.m_Format;
        textureProps.m_DebugName = Format::Format("{} Texture", filePath.stem());
        const Span<onyxU8>& imageData = file.GetData();

        m_GraphicsApi->CreateTexture(textureAsset.m_Texture, storageProps, textureProps, imageData);
        return true;
       // return Deserialize(asset, filePath);
    }

    bool TextureSerializer::DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }
}
