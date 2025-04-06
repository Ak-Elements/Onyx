#include <onyx/graphics/shader/shaderincluder.h>

#include <onyx/graphics/shader/shaderpreprocessor.h>
#include <onyx/filesystem/onyxfile.h>

namespace
{
	// From https://wandbox.org/permlink/iXC7DWaU8Tk8jrf3 and is modified.
	enum class State : char { SlashOC, StarIC, SingleLineComment, MultiLineComment, NotAComment };


	template <typename InputIt, typename OutputIt>
	void CopyWithoutComments(InputIt first, InputIt last, OutputIt out)
	{
		State state = State::NotAComment;

		while (first != last)
		{
			switch (state)
			{
			case State::SlashOC:
				if (*first == '/') state = State::SingleLineComment;
				else if (*first == '*') state = State::MultiLineComment;
				else
				{
					state = State::NotAComment;
					*out++ = '/';
					*out++ = *first;
				}
				break;
			case State::StarIC:
				if (*first == '/') state = State::NotAComment;
				else state = State::MultiLineComment;
				break;
			case State::NotAComment:
				if (*first == '/') state = State::SlashOC;
				else *out++ = *first;
				break;
			case State::SingleLineComment:
				if (*first == '\n')
				{
					state = State::NotAComment;
					*out++ = '\n';
				}
				break;
			case State::MultiLineComment:
				if (*first == '*') state = State::StarIC;
				else if (*first == '\n') *out++ = '\n';
				break;
			}
			++first;
		}
	}
}

namespace Onyx::Graphics
{
	ShaderIncluder::ShaderIncluder()
	{
	}

	ShaderIncluder::~ShaderIncluder()
	{
	}

	shaderc_include_result* ShaderIncluder::GetInclude(const char* requestedPath, shaderc_include_type type, const char* requestingPath, size_t /*includeDepth*/)
	{
		(void)requestingPath;
		using namespace FileSystem;
		const bool isRelativePath = type == shaderc_include_type_relative;

		Filepath requestedFilePath;
		if (isRelativePath)
		{
			Filepath requestingPathDirectory = requestingPath;
			requestedFilePath = Path::GetFullPath(requestingPathDirectory.parent_path() / requestedPath);
			if (Path::Exists(requestedFilePath) == false)
			{
				for (const Filepath& includeDirectory : m_IncludeDirectories)
				{
					requestedFilePath = Path::GetFullPath(includeDirectory / requestedPath);
					if (Path::Exists(requestedFilePath))
					{
						break;
					}
				}
			}
		}
		else
		{
			requestedFilePath = requestedPath;
		}

		requestedFilePath = requestedFilePath.lexically_normal();

        shaderc_include_result* const data = new shaderc_include_result;
        std::array<String, 2>* const container = new std::array<String, 2>;
		data->user_data = container;

		(*container)[0] = requestedFilePath.string();
		(*container)[1] = String();

#if ONYX_IS_DEBUG
		if (Path::Exists(requestedFilePath) == false)
		{
			ONYX_LOG_ERROR("Can not find include {} for {}", requestedPath, requestingPath);
			return data;
		}
#endif

        String shaderSource;
		if (OnyxFile::ReadAll(requestedFilePath, shaderSource, true) == false)
		{
			ONYX_LOG_ERROR("Failed reading include file {} for {}", requestedPath, requestingPath);
			return data;
		}

		std::stringstream sourceStream;	
		CopyWithoutComments(shaderSource.begin(), shaderSource.end(), std::ostream_iterator<char>(sourceStream));
		shaderSource = sourceStream.str();

		/// TODO: Add header preprocessor
		ShaderPreprocessor preprocessor;
		if (preprocessor.PreprocessShader(shaderSource) == false)
		{
			ONYX_LOG_ERROR("Failed to retrive shader entry {} for {}", requestedPath, requestingPath);
			return data;
		}

		bool isGuarded = false;
		auto [_, hasAdded] = m_Includes.emplace(requestedFilePath.string(), isGuarded, 0, shaderSource);
		if (hasAdded == false)
		{
			shaderSource.clear();
		}

		(*container)[1] = shaderSource;

		data->source_name = (*container)[0].data();
		data->source_name_length = (*container)[0].size();

		data->content = (*container)[1].data();
		data->content_length = (*container)[1].size();
		return data;
	}

	void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
	{
		delete static_cast<std::array<String, 2>*>(data->user_data);
		delete data;
	}
}
