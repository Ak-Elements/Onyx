#include <onyx/graphics/shader/shadercompiler.h>

#include <onyx/graphics/shader/shadercache.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/shader/shaderincluder.h>
#include <onyx/graphics/shader/shaderpreprocessor.h>
#include <onyx/graphics/vulkan/shader.h>
#include <onyx/log/logger.h>
#include <onyx/filesystem/onyxfile.h>

#include <shaderc/shaderc.hpp>
#include <spirv_glsl.hpp>

namespace Onyx::Graphics::ShaderCompiler
{
	// TODO: Move this to a vulkan shader compiler implementation
	namespace
	{
		constexpr StringView SHADER_SOURCE_TMP_PATH = "tmp:/shaders/source";
		constexpr StringView SHADER_BINARIES_TMP_PATH = "tmp:/shaders/bin";

		ShaderDataType GetShaderDataType(spirv_cross::SPIRType::BaseType type, onyxU32 vectorSize, onyxU32 /*componentSize*/)
		{
			using namespace spirv_cross;
		    switch (type)
			{
                case SPIRType::Boolean: 
					return ShaderDataType::Bool;
                case SPIRType::SByte:
				{
					if (vectorSize == 4)
						return ShaderDataType::Byte;

					return ShaderDataType::Byte4;
				}
                case SPIRType::UByte:
				{
					if (vectorSize == 4)
						return ShaderDataType::UByte4;

					return ShaderDataType::UByte;
				}
				case SPIRType::UInt:
				{
				    if (vectorSize == 4)
				        return ShaderDataType::UInt4;
				    if (vectorSize == 3)
				        return ShaderDataType::UInt3;
				    if (vectorSize == 2)
				        return ShaderDataType::UInt2;

				    return ShaderDataType::UInt;
				}
                case SPIRType::Float: 
				{
					if (vectorSize == 4)
						return ShaderDataType::Float4;
					if (vectorSize == 3)
						return ShaderDataType::Float3;
					if (vectorSize == 2)
						return ShaderDataType::Float2;

					return ShaderDataType::Float;
				}
		        default:
					ONYX_ASSERT(false, "Unhandeled shader base type");
					return ShaderDataType::Float;
            }
		}

		TextureFormat GetDefaultFormat(ShaderDataType type)
		{
		    switch (type)
		    {
                case ShaderDataType::Bool:
					return TextureFormat::R_UINT8;
                case ShaderDataType::Float:
					return TextureFormat::R_FLOAT32;
                case ShaderDataType::Float2:
					return TextureFormat::RG_FLOAT32;
                case ShaderDataType::Float3:
					return TextureFormat::RGB_FLOAT32;
                case ShaderDataType::Float4:
					return TextureFormat::RGBA_FLOAT32;
                case ShaderDataType::Mat3:
					return TextureFormat::RGB_UNORM8;
		        //case ShaderDataType::Mat4:
				//	return TextureFormat::RGBA_UNORM8;
                //case ShaderDataType::Byte:
				//	return TextureFormat::R_UINT8;
                //case ShaderDataType::Byte4:
				//	return TextureFormat::RGBA_UNORM8;
                //case ShaderDataType::UByte: break;
               // case ShaderDataType::UByte4: break;
               // case ShaderDataType::Short2: break;
                //case ShaderDataType::Short2Normalized: break;
                //case ShaderDataType::Short4: break;
                //case ShaderDataType::Short4Normalized: break;
                case ShaderDataType::UInt:
					return TextureFormat::R_UINT32;
                //case ShaderDataType::UInt2:
				//	return TextureFormat::RG_UINT32;
                //case ShaderDataType::UInt3:
				//	return TextureFormat::RGB_UINT32;
                //case ShaderDataType::UInt4:
				//	return TextureFormat::RGBA_UINT32;
				default:
					ONYX_ASSERT(false, "Unhandeled shader base type");
					return TextureFormat::Invalid;
            }
		}

		onyxU32 GetComponentSize(TextureFormat format)
		{
		    switch(format)
		    {
                case TextureFormat::R_UNORM8:
				case TextureFormat::R_UINT8:
		        case TextureFormat::RGB_UNORM8:
				case TextureFormat::RGBA_UNORM8:
				case TextureFormat::BGRA_UNORM8:
		        case TextureFormat::SRGB_UNORM8:
				case TextureFormat::RG_UNORM8:
					return 1;
                case TextureFormat::R_UINT16:
				case TextureFormat::RG_FLOAT16:
				case TextureFormat::RGBA_FLOAT16:
					return 2;
                case TextureFormat::R_UINT32:
                case TextureFormat::R_FLOAT32:
                case TextureFormat::RG_FLOAT32:
                case TextureFormat::RGB_FLOAT32:
                case TextureFormat::RGBA_FLOAT32:
					return 4;
				default:
					ONYX_ASSERT(false, "Unhandeled texture format.");
					return 4;
            }
		}

		bool IsReservedResourceName(const String& resourceName)
		{
			return  (resourceName == "BindlessTextures") ||
				    (resourceName == "BindlessTextures3D") ||
				    (resourceName == "BindlessTexturesCubemaps") ||
				    (resourceName == "BindlessTexturesCubemapsArray") ||
				    (resourceName == "BindlessImages2D") ||
				    (resourceName == "BindlessUImages2D");
		}

		StringView GetShaderExtension(ShaderStage stage)
		{
		    switch (stage)
		    {
            case ShaderStage::Vertex:
				return "vert";
            case ShaderStage::Fragment:
				return "frag";
            case ShaderStage::Compute:
				return "comp";
			case ShaderStage::Invalid:
            case ShaderStage::All:
		    case ShaderStage::Count:
				break;
            }

			ONYX_ASSERT(false, "Invalid shader stage {} passed", Enums::ToString(stage));
			return "";
		}

        shaderc::CompileOptions SetupShaderOptions(const GraphicsApi& api)
		{
			using namespace shaderc;
			CompileOptions shaderCOptions;
			shaderCOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
			shaderCOptions.SetWarningsAsErrors();
			shaderCOptions.AddMacroDefinition("ONYX_IS_BINDLESS", api.IsBindless() ? "1" : "0");

#if !ONYX_IS_RETAIL
			if (api.IsShaderDebugEnabled())
			{
				shaderCOptions.AddMacroDefinition("ONYX_IS_DEBUG", api.IsShaderDebugEnabled() ? "1" : "0");
				shaderCOptions.SetGenerateDebugInfo();
			}
#endif

			ShaderIncluder* includer = new ShaderIncluder();

			for (auto& [_, mountPoint] : FileSystem::Path::GetMountPoints())
			{
				const FileSystem::Filepath shaderIncludePath = mountPoint.Path / "shaders/";
				if (FileSystem::Path::Exists(shaderIncludePath))
				{
					includer->AddIncludeDirectory(shaderIncludePath.generic_string().c_str());
				}
			}

			//shaderCOptions.AddMacroDefinition("SHADERGRAPH", "test");
			shaderCOptions.SetIncluder(std::unique_ptr<ShaderIncluder>(includer));
			return shaderCOptions;
		}
	}

	namespace Glsl
	{
		bool Compile(const GraphicsApi& api, const FileSystem::Filepath& sourcePath, const String& shaderSourceCode, ShaderStage stage, DynamicArray<onyxU32>& outByteCode)
		{
			shaderc::Compiler compiler;
			shaderc::CompileOptions shaderCOptions = SetupShaderOptions(api);

			shaderc_shader_kind shaderKind = shaderc_glsl_default_vertex_shader;
			switch (stage)
			{
			    case ShaderStage::Vertex: shaderKind = shaderc_vertex_shader; break;
			    case ShaderStage::Fragment: shaderKind = shaderc_fragment_shader; break;
			    case ShaderStage::Compute: shaderKind = shaderc_compute_shader; break;
			    case ShaderStage::Invalid:
			    case ShaderStage::Count:
			    case ShaderStage::All:
				    ONYX_ASSERT(false, "Not implemented");
				    return false;
			}

			const shaderc::PreprocessedSourceCompilationResult& preProcessingResult = compiler.PreprocessGlsl(shaderSourceCode, shaderKind, sourcePath.string().c_str(), shaderCOptions);
			if (preProcessingResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				ONYX_LOG_ERROR("Failed to pre-process {} (Stage: {}) shader.\nError: {}", sourcePath, Enums::ToString(stage), preProcessingResult.GetErrorMessage());
				return false;
			}

			StringView preprocessedSource(preProcessingResult.begin(), preProcessingResult.end());

			// Compile shader
			shaderCOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
			const shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(preprocessedSource.data(), preprocessedSource.length(), shaderKind, sourcePath.string().c_str(), shaderCOptions);

			if (module.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				String test = module.GetErrorMessage();
				ONYX_LOG_ERROR("Failed to compile {} shader at stage: {}.\nError: {}", sourcePath, Enums::ToString(stage), test);
				return false;
			}

			outByteCode = DynamicArray<onyxU32>(module.begin(), module.end());

#if !ONYX_IS_RETAIL

#if ONYX_IS_WINDOWS
			// Kind of a hack because thank you Windows
#undef CreateDirectory
#endif
			// TODO: Allow different shader language and update this export folder
			String shaderStageString = ToLower(Enums::ToString(stage));
			const StringView exportShaderSourcePath = Format::Format("{}/{}/glsl/{}.{}", SHADER_SOURCE_TMP_PATH, shaderStageString, sourcePath.stem(), GetShaderExtension(stage));

			FileSystem::OnyxFile shaderSourceFile(exportShaderSourcePath);
			FileSystem::FileStream shaderSourceFileStream = shaderSourceFile.OpenStream(FileSystem::OpenMode::Text | FileSystem::OpenMode::Write);
			shaderSourceFileStream.WriteRaw(preprocessedSource.data(), preprocessedSource.size());

			const StringView exportShaderBinaryPath = Format::Format("{}/{}/glsl/{}.spirv", SHADER_BINARIES_TMP_PATH, shaderStageString, sourcePath.stem());
			FileSystem::OnyxFile shaderBinaryFile(exportShaderBinaryPath);
			FileSystem::FileStream shaderBinaryFileStream = shaderBinaryFile.OpenStream(FileSystem::OpenMode::Binary | FileSystem::OpenMode::Write);
			shaderBinaryFileStream.WriteRaw(outByteCode, false);
#endif

			return true;
		}
	}

    bool Compile(const GraphicsApi& api, const FileSystem::Filepath& sourcePath, const String& code, ShaderLanguage language, ShaderStage stage, DynamicArray<onyxU32>& outByteCode)
    {
#if !ONYX_IS_RETAIL

#if ONYX_IS_WINDOWS
		// Kind of a hack because thank you Windows
#undef CreateDirectory
#endif

		const String shaderLanguageString = ToLower(Enums::ToString(language));
		for (ShaderStage s = ShaderStage::Vertex; s < ShaderStage::All; ++s)
		{
			String shaderStageString = ToLower(Enums::ToString(s));

			const FileSystem::Filepath tempSourceDirectory = FileSystem::Path::GetFullPath(SHADER_SOURCE_TMP_PATH);
			const FileSystem::Filepath tempBinariesDirectory = FileSystem::Path::GetFullPath(SHADER_BINARIES_TMP_PATH);

			FileSystem::Path::CreateDirectory(tempSourceDirectory / shaderStageString/  shaderLanguageString);
			FileSystem::Path::CreateDirectory(tempBinariesDirectory / shaderStageString / shaderLanguageString);
		}

#endif

		switch (language)
		{
		    case ShaderLanguage::GLSL:
			    return Glsl::Compile(api, sourcePath, code, stage, outByteCode);
		    case ShaderLanguage::Invalid:
		    case ShaderLanguage::HLSL:
		    case ShaderLanguage::Count:
			    break;
		}

		ONYX_ASSERT(false, "Shader compiler not implemented for language ({}).", Enums::ToString(language));
		return false;
    }

	bool Reflect(ShaderStage shaderStage, const PreprocessedShader& preprocessedShader, const DynamicArray<onyxU32>& shaderByteCode, ShaderReflectionInfo& outReflectionInfo)
	{
		ONYX_UNUSED(preprocessedShader);
		// TODO: Clean up & add support for alignment calculation for push constants for example
		using namespace spirv_cross;

		Compiler compiler(shaderByteCode);
		const ShaderResources& resources = compiler.get_shader_resources();

		if (shaderStage == ShaderStage::Vertex)
		{
			DynamicArray<VertexInput> inputs;
			for (const spirv_cross::Resource& resource : resources.stage_inputs)
			{
				const String& name = resource.name;
				const SPIRType& inputType = compiler.get_type(resource.base_type_id);

				onyxU32 location = compiler.get_decoration(resource.id, spv::DecorationLocation);

				StringId32 id(name);
				onyxU32 componentSize = inputType.width;
				TextureFormat format = TextureFormat::Invalid;
				auto keyValueIt = preprocessedShader.m_Formats.find(id);
				if (keyValueIt != preprocessedShader.m_Formats.end())
				{
					format = keyValueIt->second;
					componentSize = GetComponentSize(format);
				}

				if (format == TextureFormat::Invalid)
				{
					ShaderDataType type = GetShaderDataType(inputType.basetype, inputType.vecsize, componentSize);
					format = GetDefaultFormat(type);
				}

				outReflectionInfo.vertexInput.Add(location, format);
			}
		}

		// Get out format for pixel shader
		if (shaderStage == ShaderStage::Fragment)
		{
			//DynamicArray<VertexInput> inputs;
			for (const spirv_cross::Resource& resource : resources.stage_outputs)
			{
				const String& name = resource.name;

				const SPIRType& inputType = compiler.get_type(resource.base_type_id);

				//onyxU32 location = compiler.get_decoration(resource.id, spv::DecorationLocation);

				StringId32 id(name);
				onyxU32 componentSize = inputType.width;
				TextureFormat format = TextureFormat::Invalid;
				auto keyValueIt = preprocessedShader.m_Formats.find(id);
				if (keyValueIt != preprocessedShader.m_Formats.end())
				{
					format = keyValueIt->second;
					componentSize = GetComponentSize(format);
				}

				if (format == TextureFormat::Invalid)
				{
					ShaderDataType type = GetShaderDataType(inputType.basetype, inputType.vecsize, componentSize);
					format = GetDefaultFormat(type);
				}

				outReflectionInfo.OutputAttachments[name] = format;
			}
		}

		for (const spirv_cross::Resource& resource : resources.uniform_buffers)
		{
			const SmallVector<BufferRange>& activeBuffers = compiler.get_active_buffer_ranges(resource.id);
			// Discard unused buffers from headers
			if (activeBuffers.empty() == false)
			{
				const String& name = resource.name;
				const String& alias = compiler.get_name(resource.id);
				const SPIRType& bufferType = compiler.get_type(resource.base_type_id);

				onyxU32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				onyxU8 descriptorSet = numeric_cast<onyxU8>(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet));
				onyxU32 size = static_cast<onyxU32>(compiler.get_declared_struct_size(bufferType));
				if (IsReservedDescriptorSet(descriptorSet))
				{
					if (IsReservedResourceName(name) == false)
					{
						ONYX_LOG_ERROR("Found a resource that is using the a reserved name and descriptor set. Name: {}", name);
					}

					continue;
				}

				ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(static_cast<onyxS32>(descriptorSet), outReflectionInfo);
				UniformBuffer uniformBuffer;
				uniformBuffer.Id = StringId32(alias);
				uniformBuffer.BindingPoint = binding;
				uniformBuffer.Size = size;
				uniformBuffer.Stage = ShaderStage::All;

				shaderDescriptorSet.UniformBuffers[binding] = uniformBuffer;
			}
		}

		for (const spirv_cross::Resource& resource : resources.storage_buffers)
		{
			const SmallVector<BufferRange>& activeBuffers = compiler.get_active_buffer_ranges(resource.id);
			// Discard unused buffers from headers
			if (activeBuffers.empty() == false)
			{
				const String& name = resource.name;
				const String& alias = compiler.get_name(resource.id);
				const SPIRType& bufferType = compiler.get_type(resource.base_type_id);
				onyxU32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				onyxU8 descriptorSet = numeric_cast<onyxU8>(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet));
				onyxU32 size = static_cast<onyxU32>(compiler.get_declared_struct_size(bufferType));

				if (IsReservedDescriptorSet(descriptorSet))
				{
					if (IsReservedResourceName(name) == false)
					{
						ONYX_LOG_ERROR("Found a resource that is using the a reserved name and descriptor set. Name: {}", name);
					}

					continue;
				}

				ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(static_cast<onyxS32>(descriptorSet), outReflectionInfo);
				StorageBuffer storageBuffer;
				storageBuffer.Id = alias.empty() ? StringId32(name) : StringId32(alias);
				storageBuffer.BindingPoint = binding;
				storageBuffer.Size = size;
				storageBuffer.Stage = ShaderStage::All;
				shaderDescriptorSet.StorageBuffers[binding] = storageBuffer;
			}
		}

		for (const spirv_cross::Resource& resource : resources.push_constant_buffers)
		{
			const String& bufferName = resource.name;
			const SPIRType& bufferType = compiler.get_type(resource.base_type_id);
			onyxU32 bufferSize = static_cast<onyxU32>(compiler.get_declared_struct_size(bufferType));
			onyxU32 memberCount = static_cast<onyxU32>(bufferType.member_types.size());
			onyxU32 bufferOffset = 0;

			if (outReflectionInfo.pushConstantRanges.empty() == false)
			{
				const PushConstantRange& last = outReflectionInfo.pushConstantRanges.back();
				bufferOffset = last.Offset + last.Size;
			}

			PushConstantRange& pushConstantRange = outReflectionInfo.pushConstantRanges.emplace_back();
			pushConstantRange.Stage = shaderStage;
			pushConstantRange.Size = bufferSize - bufferOffset;
			pushConstantRange.Offset = bufferOffset;

			ShaderBuffer& buffer = outReflectionInfo.constantBuffers[bufferName];
			buffer.Name = bufferName;
			buffer.Size = bufferSize - bufferOffset;

			for (uint32_t i = 0; i < memberCount; i++)
			{
				const String& memberName = compiler.get_member_name(bufferType.self, i);

				onyxU32 size = static_cast<onyxU32>(compiler.get_declared_struct_member_size(bufferType, i));
				onyxU32 offset = compiler.type_struct_member_offset(bufferType, i) - bufferOffset;

				String uniformName = Format::Format("{}.{}", bufferName, memberName);
				buffer.Uniforms[uniformName] = ShaderUniform(uniformName, size, offset);
			}
		}

		for (const spirv_cross::Resource& resource : resources.sampled_images)
		{
			const String& name = resource.name;
			const SPIRType& baseType = compiler.get_type(resource.base_type_id);
			const SPIRType& type = compiler.get_type(resource.type_id);
			onyxU32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			onyxU8 descriptorSet = numeric_cast<onyxU8>(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet));
			onyxU32 dimension = baseType.image.dim;
			onyxU32 arraySize = type.array.empty() ? 1 : type.array[0];

			if (IsReservedDescriptorSet(descriptorSet))
			{
				if (IsReservedResourceName(name) == false)
				{
					ONYX_LOG_ERROR("Found a resource that is using the a reserved name and descriptor set. Name: {}", name);
				}

				continue;
			}

			ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(static_cast<onyxS32>(descriptorSet), outReflectionInfo);
			ImageSampler& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
			imageSampler.BindingPoint = binding;
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.Stage = shaderStage;
			imageSampler.Dimension = dimension;
			imageSampler.ArraySize = arraySize;

			outReflectionInfo.shaderResources[name] = ShaderResourceDeclaration(name, descriptorSet, binding, 1);
		}

		for (const spirv_cross::Resource& resource : resources.separate_images)
		{
			const String& name = resource.name;
			const SPIRType& baseType = compiler.get_type(resource.base_type_id);
			const SPIRType& type = compiler.get_type(resource.type_id);
			onyxU32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			onyxU8 descriptorSet = numeric_cast<onyxU8>(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet));
			onyxU32 dimension = baseType.image.dim;
			onyxU32 arraySize = type.array.empty() ? 1 : type.array[0];

			if (IsReservedDescriptorSet(descriptorSet))
			{
				if (IsReservedResourceName(name) == false)
				{
					ONYX_LOG_ERROR("Found a resource that is using the a reserved name and descriptor set. Name: {}", name);
				}

				continue;
			}

			ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(static_cast<onyxS32>(descriptorSet), outReflectionInfo);
			ImageSampler& imageSampler = shaderDescriptorSet.SeparateTextures[binding];
			imageSampler.BindingPoint = binding;
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.Stage = shaderStage;
			imageSampler.Dimension = dimension;
			imageSampler.ArraySize = arraySize;

			outReflectionInfo.shaderResources[name] = ShaderResourceDeclaration(name, descriptorSet, binding, 1);
		}

		for (const spirv_cross::Resource& resource : resources.separate_samplers)
		{
			const String& name = resource.name;
			const SPIRType& baseType = compiler.get_type(resource.base_type_id);
			const SPIRType& type = compiler.get_type(resource.type_id);
			onyxU32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			onyxU8 descriptorSet = numeric_cast<onyxU8>(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet));
			onyxU32 dimension = baseType.image.dim;
			onyxU32 arraySize = type.array.empty() ? 1 : type.array[0];

			if (IsReservedDescriptorSet(descriptorSet))
			{
				if (IsReservedResourceName(name) == false)
				{
					ONYX_LOG_ERROR("Found a resource that is using the a reserved name and descriptor set. Name: {}", name);
				}

				continue;
			}

			ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(static_cast<onyxS32>(descriptorSet), outReflectionInfo);
			auto& imageSampler = shaderDescriptorSet.SeparateSamplers[binding];
			imageSampler.BindingPoint = binding;
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.Stage = shaderStage;
			imageSampler.Dimension = dimension;
			imageSampler.ArraySize = arraySize;

			outReflectionInfo.shaderResources[name] = ShaderResourceDeclaration(name, descriptorSet, binding, 1);
		}

		for (const spirv_cross::Resource& resource : resources.storage_images)
		{
			const String& name = resource.name;
			const SPIRType& type = compiler.get_type(resource.type_id);
			onyxU32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			onyxU8 descriptorSet = numeric_cast<onyxU8>(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet));
			onyxU32 dimension = type.image.dim;
			onyxU32 arraySize = type.array.empty() ? 1 : type.array[0];

			if (IsReservedDescriptorSet(descriptorSet))
			{
				if (IsReservedResourceName(name) == false)
				{
					ONYX_LOG_ERROR("Found a resource that is using the a reserved name and descriptor set. Name: {}", name);
				}

				continue;
			}

			ShaderDescriptorSet& shaderDescriptorSet = GetOrCreateShaderDescriptorSet(static_cast<onyxS32>(descriptorSet), outReflectionInfo);
			ImageSampler& imageSampler = shaderDescriptorSet.StorageImages[binding];
			imageSampler.BindingPoint = binding;
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.Dimension = dimension;
			imageSampler.ArraySize = arraySize;
			imageSampler.Stage = shaderStage;

			outReflectionInfo.shaderResources[name] = ShaderResourceDeclaration(name, descriptorSet, binding, 1);
		}

		return true;
	}

	bool IsReservedDescriptorSet(onyxU8 set)
	{
		return set == BINDLESS_SET;
	}

	ShaderDescriptorSet& GetOrCreateShaderDescriptorSet(onyxU8 setIndex, ShaderReflectionInfo& reflectionInfo)
	{
		auto it = std::find_if(reflectionInfo.shaderDescriptorSets.begin(), reflectionInfo.shaderDescriptorSets.end(), [&](ShaderDescriptorSet& descriptorSet)
			{
				return setIndex == descriptorSet.Set;
			});

		if (it != reflectionInfo.shaderDescriptorSets.end())
		{
			return *it;
		}

		return reflectionInfo.shaderDescriptorSets.emplace_back(setIndex);
	}

    bool ValidateCode(const GraphicsApi& graphicsApi, const String& shaderSourceCode)
    {
		shaderc::Compiler compiler;
		shaderc::CompileOptions shaderCOptions = SetupShaderOptions(graphicsApi);
		shaderCOptions.SetForcedVersionProfile(ShaderCoreVersion, shaderc_profile_core);
		const shaderc::PreprocessedSourceCompilationResult& preProcessingResult = compiler.PreprocessGlsl(shaderSourceCode, shaderc_glsl_default_vertex_shader, "validation", shaderCOptions);
		if (preProcessingResult.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			ONYX_LOG_ERROR("Validation for shader source failed. \nError: {}", preProcessingResult.GetErrorMessage());
			return false;
		}

		return true;
    }
}
