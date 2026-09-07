#include <onyx/filesystem/onyxfile.h>

#include <filesystem>

#include <nlohmann/json.hpp>

namespace onyx::file_system {
OnyxFile::OnyxFile( const FilePath& filePath )
    : m_filePath( filePath )
    , m_fileId( hash::fnV1aHash< uint64_t >( filePath.string() ) ) {}

OnyxFile::OnyxFile( StringView mountPath )
    : m_filePath( path::getFullPath( mountPath ) )
    , m_fileId( hash::fnV1aHash< uint64_t >( m_filePath.string() ) ) {}

bool OnyxFile::writeAll( const FilePath& filePath, StringView content ) {
    FileStream fileStream( filePath, OpenMode::Write | OpenMode::Text );
    if( fileStream.isValid() == false )
        return false;
    fileStream.writeRaw( content );
    return true;
}

bool OnyxFile::readAll( const FilePath& filePath, String& outFileContent ) {
    return readAll( filePath, outFileContent, false );
}

bool OnyxFile::readAll( const FilePath& filePath, String& outFileContent, bool shouldSkipBOM ) {
    FileStream fileStream( filePath, OpenMode::Read );
    if( fileStream.isValid() == false )
        return false;

    if( shouldSkipBOM )
        fileStream.readAllWithoutBOM( outFileContent );
    else
        fileStream.readAll( outFileContent );

    return true;
}

FileStream OnyxFile::openStream( OpenMode mode ) const {
    return { path::getWorkingDirectory() / m_filePath, mode };
}

JsonValue OnyxFile::loadJson() const {
    // for convenience
    using json = nlohmann::ordered_json;
    FileStream stream = openStream( OpenMode::Read );

    // TODO: use a SAX parser to get proper errors
    json data = json::parse( stream.get(), nullptr, false );
    return { data };
}

void OnyxFile::writeJson( const JsonValue& json ) const {
    FileStream stream = openStream( OpenMode::Write | OpenMode::Text );
    const String& jsonString = json.Json.dump( 4 );
    stream.writeRaw( jsonString.data(), jsonString.size() );
}
} // namespace onyx::file_system
