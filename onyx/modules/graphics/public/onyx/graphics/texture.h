#pragma once

#include <onyx/graphics/textureproperties.h>
#include <onyx/graphics/graphicstypes.h>

namespace Onyx::Graphics
{
    class GraphicsApi;
    class TextureStorage;

	namespace Vulkan
	{
		class VulkanGraphicsApi;
	}

	namespace Internal
    {
	    template <typename T>
	    struct TextureDeleter;
	}

    class Texture : public RefCounted
	{
		friend class Vulkan::VulkanGraphicsApi;
		friend struct Internal::TextureDeleter<Texture>;
	public:
		Texture(const TextureProperties& properties, const TextureStorage* storage);
        ~Texture() override = default;

		TextureProperties& GetProperties() { return m_Properties; }
		const TextureProperties& GetProperties() const { return m_Properties; }

		const TextureStorage& GetStorage() const { ONYX_ASSERT(m_Storage != nullptr); return *m_Storage; }

		onyxU32 GetIndex() const { return m_Index; }

    private:
		void SetIndex(onyxU32 index) { m_Index = index; }

		virtual void Release() = 0;

	protected:
		onyxU32 m_Index = onyxMax_U32; // index in memory pool and bindless texture index
		TextureProperties m_Properties;
		const TextureStorage* m_Storage; // non owning
	};

	namespace Internal
	{
		template <typename T>
		struct TextureDeleter
		{
			TextureDeleter() = default;

			template <typename _Ty2, std::enable_if_t<std::is_convertible_v<_Ty2*, T*>, int> = 0>
			constexpr  TextureDeleter(const TextureDeleter<_Ty2>&) noexcept {}

			void operator()(T* texture) const
			{
				// if no index is set the texture is not from the bindless pool delete normally
				if (texture->GetIndex() == onyxMax_U32)
				{
					ONYX_SAFE_DELETE(texture);
				}
				else
				{
					texture->Release();
				}
			}
		};
	}

	using TextureDeleter = Internal::TextureDeleter<Texture>;

	namespace Utils
    {
		inline bool IsDepthFormat(TextureFormat format)
		{
			return (format == TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT) ||
				(format == TextureFormat::DEPTH_FLOAT32) ||
				(format == TextureFormat::DEPTH_STENCIL_UNORM24_8UINT) ||
				(format == TextureFormat::DEPTH_STENCIL_UNORM16_8UINT) ||
				(format == TextureFormat::DEPTH_UNORM16) ||
				(format == TextureFormat::STENCIL_UINT8);// is this valid?
		}

		inline bool HasStencil(TextureFormat format)
		{
			return (format == TextureFormat::STENCIL_UINT8) ||
				(format == TextureFormat::DEPTH_STENCIL_UNORM16_8UINT) ||
				(format == TextureFormat::DEPTH_STENCIL_UNORM24_8UINT) ||
				(format == TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT);
		}

		inline onyxU32 GetImageFormatBPP(TextureFormat format)
		{
			switch (format)
			{
			    case TextureFormat::R_UNORM8:  return 1;
			    case TextureFormat::R_UINT8:  return 1;
			    case TextureFormat::R_UINT16: return 2;
			    case TextureFormat::R_UINT32: return 4;
			    case TextureFormat::R_FLOAT32:  return 4;
			    case TextureFormat::RGB_UNORM8:
			    case TextureFormat::SRGB_UNORM8:    return 3;
			    case TextureFormat::RGBA_UNORM8:    return 4;
			    case TextureFormat::RGBA_FLOAT16: return 2 * 4;
			    case TextureFormat::RGBA_FLOAT32: return 4 * 4;
			    case TextureFormat::RGB_UFLOAT32_PACKED_11_11_10: return 4;
			}
			ONYX_ASSERT(false, "Texture format not implemented.");
			return 0;
		}

		inline bool IsIntegerBased(const TextureFormat format)
		{
			switch (format)
			{
			    case TextureFormat::R_UINT8:
			    case TextureFormat::R_UINT16:
			    case TextureFormat::R_UINT32:
			    case TextureFormat::DEPTH_STENCIL_FLOAT32_8UINT:
			    case TextureFormat::STENCIL_UINT8:
				    return true;
			    case TextureFormat::DEPTH_FLOAT32:
			    case TextureFormat::R_UNORM8:
			    case TextureFormat::RGB_UFLOAT32_PACKED_11_11_10:
			    case TextureFormat::RG_FLOAT16:
			    case TextureFormat::RG_FLOAT32:
			    case TextureFormat::R_FLOAT32:
			    case TextureFormat::RG_UNORM8:
			    case TextureFormat::RGBA_FLOAT32:
			    case TextureFormat::RGBA_FLOAT16:
			    case TextureFormat::RGB_UNORM8:
			    case TextureFormat::RGBA_UNORM8:
			    case TextureFormat::SRGB_UNORM8:
			    case TextureFormat::DEPTH_STENCIL_UNORM24_8UINT:
			    case TextureFormat::DEPTH_STENCIL_UNORM16_8UINT:
				    return false;
			}

			ONYX_ASSERT(false, "Texture format not implemented.");
			return false;
		}

		inline onyxU32 CalculateMipCount(onyxU32 width, onyxU32 height)
		{
			return static_cast<onyxU32>(std::log2(std::min(width, height))) + 1;
		}

		inline onyxU32 GetImageMemorySize(TextureFormat format, onyxU32 width, onyxU32 height)
		{
			return width * height * GetImageFormatBPP(format);
		}

	}
}
