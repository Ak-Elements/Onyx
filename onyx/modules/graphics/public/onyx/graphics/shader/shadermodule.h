#pragma once

#include <onyx/graphics/graphicstypes.h>

#include <onyx/stream/stream.h>

namespace Onyx::Graphics
{
	struct UniformBuffer
	{
        onyxU32 Id = 0; // Hashed Name
        onyxU32 BindingPoint = 0;
        onyxU32 Size = 0;
        TextureFormat Format;
        ShaderStage Stage = ShaderStage::Invalid;

		//TODO: Should be left out in release builds
        String Name;

        void Serialize(Stream& outStream) const
		{
			outStream.Write(Id);
            outStream.Write(BindingPoint);
            outStream.Write(Size);
            outStream.Write(Format);
            outStream.Write(Stage);

            outStream.Write(Name);
        }

        void Deserialize(const Stream& inStream)
		{
			inStream.Read(Id);
            inStream.Read(BindingPoint);
            inStream.Read(Size);
            inStream.Read(Format);
            inStream.Read(Stage);

            inStream.Read(Name);
        }
	};

	struct StorageBuffer
	{
        onyxU32 Id = 0; // Hashed Name
        onyxU32 BindingPoint = 0;
        onyxU32 Size = 0;
        ShaderStage Stage = ShaderStage::Invalid;

		//TODO: Should be left out in release builds
        String Name;

        void Serialize(Stream& outStream) const
		{
			outStream.Write(Id);
            outStream.Write(BindingPoint);
            outStream.Write(Size);
            outStream.Write(Stage);

            outStream.Write(Name);
        }

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(Id);
            inStream.Read(BindingPoint);
            inStream.Read(Size);
            inStream.Read(Stage);

            inStream.Read(Name);
        }
	};

	struct ImageSampler
	{
		onyxU32 BindingPoint = 0;
		onyxU32 DescriptorSet = 0;
		onyxU32 ArraySize = 0;
		onyxU32 Dimension = 0;
		String Name;
		ShaderStage Stage = ShaderStage::Invalid;

		void Serialize(Stream& outStream) const
		{
            outStream.Write(BindingPoint);
			outStream.Write(DescriptorSet);
			outStream.Write(ArraySize);
			outStream.Write(Dimension);
			outStream.Write(Name);
			outStream.Write(Stage);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(BindingPoint);
			inStream.Read(DescriptorSet);
			inStream.Read(ArraySize);
			inStream.Read(Dimension);
			inStream.Read(Name);
			inStream.Read(Stage);
		}
	};

	struct PushConstantRange
	{
		ShaderStage Stage = ShaderStage::Invalid;
		onyxU32 Offset = 0;
		onyxU32 Size = 0;

		void Serialize(Stream& outStream) const
		{
			outStream.Write(Stage);
			outStream.Write(Offset);
			outStream.Write(Size);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(Stage);
			inStream.Read(Offset);
			inStream.Read(Size);
		}
	};

	struct ShaderUniform
	{
		String Name;
		onyxU32 Size = 0;
		onyxU32 Offset = 0;

		void Serialize(Stream& outStream) const
		{
			outStream.Write(Name);
			outStream.Write(Size);
			outStream.Write(Offset);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(Name);
			inStream.Read(Size);
			inStream.Read(Offset);
		}
	};

	struct ShaderBuffer
	{
		String Name;
		onyxU32 Size = 0;
		HashMap<String, ShaderUniform> Uniforms;

		void Serialize(Stream& outStream) const
		{
			outStream.Write(Name);
			outStream.Write(Size);
			outStream.Write(Uniforms);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(Name);
			inStream.Read(Size);
			inStream.Read(Uniforms);
		}
	};

	struct ShaderResourceDeclaration
	{
		String Name;
		onyxU32 Set = 0;
		onyxU32 Register = 0;
		onyxU32 Count = 0;

		void Serialize(Stream& outStream) const
		{
			outStream.Write(Name);
			outStream.Write(Set);
			outStream.Write(Register);
			outStream.Write(Count);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(Name);
			inStream.Read(Set);
			inStream.Read(Register);
			inStream.Read(Count);
		}
	};
	
	struct ShaderDescriptorSet
	{
		onyxU8 Set;

		HashMap<onyxU32, UniformBuffer> UniformBuffers;
		HashMap<onyxU32, StorageBuffer> StorageBuffers;
		HashMap<onyxU32, ImageSampler> ImageSamplers;
		HashMap<onyxU32, ImageSampler> StorageImages;
		HashMap<onyxU32, ImageSampler> SeparateTextures; // Not really an image sampler.
		HashMap<onyxU32, ImageSampler> SeparateSamplers;

		void Serialize(Stream& outStream) const
		{
			outStream.Write(Set);
			outStream.Write(UniformBuffers);
			outStream.Write(StorageBuffers);
			outStream.Write(ImageSamplers);
			outStream.Write(StorageImages);
			outStream.Write(SeparateTextures);
			outStream.Write(SeparateSamplers);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(Set);
			inStream.Read(UniformBuffers);
			inStream.Read(StorageBuffers);
			inStream.Read(ImageSamplers);
			inStream.Read(StorageImages);
			inStream.Read(SeparateTextures);
			inStream.Read(SeparateSamplers);
		}
	};

	struct VertexInput
	{
		TextureFormat Format;
		onyxU32 Location;

        bool operator<(const VertexInput& other) const
        {
            return Location < other.Location;
        }

		void Serialize(Stream& outStream) const
		{
			outStream.Write(Format);
			outStream.Write(Location);
		}

		void Deserialize(const Stream& outStream)
		{
			outStream.Read(Format);
			outStream.Read(Location);
		}
    };

	struct VertexInputStream
	{
		onyxU32 GetStride() const { return m_Stride; }
		const Set<VertexInput>& GetInputs() const { return m_Inputs; }
        void Add(onyxU32 location, TextureFormat format)
		{
			m_Inputs.emplace(format, location);
		}

		void Serialize(Stream& outStream) const
		{
			outStream.Write(m_Stride);
			outStream.Write<Set>(m_Inputs);
		}

		void Deserialize(const Stream& outStream)
		{
			outStream.Read(m_Stride);
			outStream.Read(m_Inputs);
		}

    private:
		onyxU32 m_Stride = 0;
        Set<VertexInput> m_Inputs;
    };

    struct ShaderReflectionInfo
	{
		bool IsUsingBindless = false;

		VertexInputStream vertexInput;
		DynamicArray<ShaderDescriptorSet> shaderDescriptorSets;
		DynamicArray<PushConstantRange> pushConstantRanges;
		
		//TODO: Do we need the string here or could this be a hash?
		HashMap<String, ShaderResourceDeclaration> shaderResources;
		HashMap<String, ShaderBuffer> constantBuffers;
		HashMap<String, TextureFormat> OutputAttachments;

		void Serialize(Stream& outStream) const
		{
			outStream.Write(IsUsingBindless);
			outStream.Write(vertexInput);
			outStream.Write<DynamicArray>(shaderDescriptorSets);
			outStream.Write<DynamicArray>(pushConstantRanges);
			outStream.Write(shaderResources);
			outStream.Write(constantBuffers);
		}

		void Deserialize(const Stream& inStream)
		{
			inStream.Read(IsUsingBindless);
			inStream.Read(vertexInput);
			inStream.Read<DynamicArray>(shaderDescriptorSets);
			inStream.Read<DynamicArray>(pushConstantRanges);
			inStream.Read(shaderResources);
			inStream.Read(constantBuffers);
		}
	};

	class Shader : public RefCounted
    {
    public:
		using ByteCode = DynamicArray<onyxU32>;
		using PerStageByteCodes = InplaceArray<ByteCode, MAX_SHADER_STAGES>;

		virtual bool AddStage(ShaderStage stage, const ByteCode& byteCode) = 0;
		virtual void RemoveStage(ShaderStage stage) = 0;

		virtual const ShaderReflectionInfo& GetReflectionData() const = 0;
	    virtual bool UpdateReflectionData(ShaderReflectionInfo& reflectionInfo) = 0;

		virtual onyxU64 GetShaderHash() const = 0;
		virtual void SetShaderHash(onyxU64 hash) = 0;

		virtual bool IsComputeShader() const = 0;
		virtual bool HasDescriptorSetLayout() const = 0;
    };
}
