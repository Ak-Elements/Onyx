#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/rhi/shader/shadercache.h>
#include <shaderc/shaderc.hpp>

namespace onyx::rhi
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
	struct hash<onyx::rhi::ShaderInclude>
	{
		size_t operator()(const onyx::rhi::ShaderInclude& data) const noexcept
		{
			return std::filesystem::hash_value(data.IncludedFilePath) ^ data.HashValue;
		}
	};
}

namespace onyx::rhi
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

		HashSet<String> GetIncludes() const;

    private:
		HashSet<FilePath> m_IncludeDirectories;
		HashSet<ShaderInclude> m_Includes;
		HashSet<String> m_ParsedSpecialMacros;
	};
}
