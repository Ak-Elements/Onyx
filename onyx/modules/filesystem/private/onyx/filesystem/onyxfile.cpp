#include <onyx/filesystem/onyxfile.h>

#include <filesystem>

#include <nlohmann/json.hpp>

namespace Onyx::FileSystem
{
    OnyxFile::OnyxFile(const std::filesystem::path& filePath)
        : m_FileId(Hash::FNV1aHash64(filePath.string()))
        , m_FilePath(filePath)
        , m_Type(Type::Raw)
    {
    }

    bool OnyxFile::ReadAll(const FileSystem::Filepath& filePath, String& outFileContent)
    {
        return ReadAll(filePath, outFileContent, false);
    }

    bool OnyxFile::ReadAll(const Filepath& filePath, String& outFileContent, bool shouldSkipBOM)
    {
        FileStream fileStream(filePath, OpenMode::Read);
        if (fileStream.IsValid() == false)
            return false;

        if (shouldSkipBOM)
            fileStream.ReadAllWithoutBOM(outFileContent);
        else
            fileStream.ReadAll(outFileContent);

        return true;
    }

    FileStream OnyxFile::OpenStream(OpenMode mode) const
    {
        return { Path::GetFullPath(m_FilePath), mode };
    }

    JsonValue OnyxFile::LoadJson() const
    {
        // for convenience
        using json = nlohmann::ordered_json;
        FileStream stream = OpenStream(OpenMode::Read);

        //TODO: use a SAX parser to get proper errors
        json data = json::parse(stream.get(), nullptr, false);
        return { data };
    }

    void OnyxFile::WriteJson(const JsonValue& json) const
    {
        FileStream stream = OpenStream(OpenMode::Write | OpenMode::Text);
        const String& jsonString = json.Json.dump(4);
        stream.WriteRaw(jsonString.data(), jsonString.size());
    }
}
