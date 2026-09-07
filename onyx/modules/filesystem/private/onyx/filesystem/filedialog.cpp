#include <onyx/filesystem/filedialog.h>

#include <nfd.h>
namespace onyx::file_system {
void FileDialog::init() {
    NFD_Init();
}

void FileDialog::shutdown() {
    NFD_Quit();
}

bool FileDialog::openFileDialog( FilePath& outPath,
                                 StringView extensionName,
                                 const DynamicArray< StringView >& extensions ) {
    nfdu8char_t* path;
    nfdopendialogu8args_t args{ nullptr, 0, nullptr, { 0, nullptr } };

    DynamicArray< nfdu8filteritem_t > filters;
    for( StringView extension : extensions ) {
        filters.emplace_back( extensionName.data(), extension.data() );
    }

    const String workingDirectory = path::getWorkingDirectory().string();
    args.defaultPath = workingDirectory.c_str();
    args.filterList = filters.data();
    args.filterCount = static_cast< nfdfiltersize_t >( filters.size() );
    nfdresult_t result = NFD_OpenDialogU8_With( &path, &args );

    if( result != NFD_OKAY ) {
        return false;
    }

    outPath = path::convertToMountPath( path );
    NFD_FreePathU8( path );
    return true;
}

bool FileDialog::saveFileDialog( FilePath& outPath,
                                 StringView extensionName,
                                 const DynamicArray< StringView >& extensions ) {
    nfdu8char_t* path;
    nfdsavedialogu8args_t args{ nullptr, 0, nullptr, "", { 0, nullptr } };

    DynamicArray< nfdu8filteritem_t > filters;
    for( StringView extension : extensions ) {
        filters.emplace_back( extensionName.data(), extension.data() );
    }

    const String workingDirectory = path::getWorkingDirectory().string();
    args.defaultPath = workingDirectory.c_str();
    args.filterList = filters.data();
    args.filterCount = static_cast< nfdfiltersize_t >( filters.size() );

    nfdresult_t result = NFD_SaveDialogU8_With( &path, &args );

    if( result != NFD_OKAY ) {
        return false;
    }

    outPath = path::convertToMountPath( path );
    NFD_FreePathU8( path );
    return true;
}
} // namespace onyx::file_system
