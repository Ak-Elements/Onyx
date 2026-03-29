#include <onyx/filesystem/onyxfile.h>

#include <filesystem>

#include <nlohmann/json.hpp>

namespace onyx::file_system {
OnyxFile::OnyxFile( const FilePath& filePath )
    : m_FilePath( filePath )
    , m_FileId( hash::fnV1aHash< uint64_t >( filePath.string() ) ) {}

OnyxFile::OnyxFile( StringView mountPath )
    : m_FilePath( path::getFullPath( mountPath ) )
    , m_FileId( hash::fnV1aHash< uint64_t >( m_FilePath.string() ) ) {}

bool OnyxFile::ReadAll( const FilePath& filePath, String& outFileContent ) {
    return ReadAll( filePath, outFileContent, false );
}

bool OnyxFile::ReadAll( const FilePath& filePath, String& outFileContent, bool shouldSkipBOM ) {
    FileStream fileStream( filePath, OpenMode::Read );
    if ( fileStream.isValid() == false )
        return false;

    if ( shouldSkipBOM )
        fileStream.readAllWithoutBOM( outFileContent );
    else
        fileStream.readAll( outFileContent );

    return true;
}

FileStream OnyxFile::OpenStream( OpenMode mode ) const {
    return { path::getWorkingDirectory() / m_FilePath, mode };
}

JsonValue OnyxFile::LoadJson() const {
    // for convenience
    using json = nlohmann::ordered_json;
    FileStream stream = OpenStream( OpenMode::Read );

    // TODO: use a SAX parser to get proper errors
    json data = json::parse( stream.get(), nullptr, false );
    return { data };
}

void OnyxFile::WriteJson( const JsonValue& json ) const {
    FileStream stream = OpenStream( OpenMode::Write | OpenMode::Text );
    const String& jsonString = json.Json.dump( 4 );
    stream.writeRaw( jsonString.data(), jsonString.size() );
}
} // namespace onyx::file_system
