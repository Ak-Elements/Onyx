#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/typetraits.h>

namespace onyx::file_system {
class FileDialog {
  public:
    static void init();
    static void shutdown();

    template < typename T >
    static bool openFileDialog( FilePath& outPath, StringView extensionName, const T& extensions ) {
        DynamicArray< StringView > extensionsList;
        for( StringView extension : extensions ) {
            extensionsList.push_back( extension );
        }

        return openFileDialog( outPath, extensionName, extensionsList );
    }

    template < typename T >
    static bool saveFileDialog( FilePath& outPath, StringView extensionName, const T& extensions ) {
        DynamicArray< StringView > extensionsList;
        for( StringView extension : extensions ) {
            extensionsList.push_back( extension );
        }
        return saveFileDialog( outPath, extensionName, extensionsList );
    }

    static bool openFileDialog( FilePath& outPath,
                                StringView extensionName,
                                const DynamicArray< StringView >& extensions );
    static bool saveFileDialog( FilePath& outPath,
                                StringView extensionName,
                                const DynamicArray< StringView >& extensions );
};
} // namespace onyx::file_system
