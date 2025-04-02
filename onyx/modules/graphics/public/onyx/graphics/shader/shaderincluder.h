#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/graphics/shader/shadercache.h>
#include <shaderc/shaderc.hpp>

namespace Onyx::Graphics
{
	struct ShaderInclude
	{
		String IncludedFilePath{};
		bool IsGuarded = false;
		onyxU64 HashValue{};
		String Code;

		bool operator==(const ShaderInclude& other) const noexcept
		{
			return IncludedFilePath == other.IncludedFilePath && HashValue == other.HashValue;
		}
	};
}

namespace std
{
	template<>
	struct hash<Onyx::Graphics::ShaderInclude>
	{
		size_t operator()(const Onyx::Graphics::ShaderInclude& data) const noexcept
		{
			return std::filesystem::hash_value(data.IncludedFilePath) ^ data.HashValue;
		}
	};
}

namespace Onyx::Graphics
{
	class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
	{
	public:
		explicit ShaderIncluder();
		~ShaderIncluder() override;

		// shaderc::CompileOptions::IncluderInterface interface
		shaderc_include_result* GetInclude(const char* requestedPath, shaderc_include_type type, const char* requestingPath, size_t includeDepth) override;
		void ReleaseInclude(shaderc_include_result* data) override;
		void AddIncludeDirectory(const char* str) { m_IncludeDirectories.emplace(str); }

	private:
		HashSet<FileSystem::Filepath> m_IncludeDirectories;
		HashSet<ShaderInclude> m_Includes;
		HashSet<String> m_ParsedSpecialMacros;
	};
}