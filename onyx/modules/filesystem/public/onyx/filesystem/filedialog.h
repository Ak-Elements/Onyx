#pragma once

#include <onyx/typetraits.h>
#include <onyx/filesystem/path.h>

namespace onyx::file_system
{
    class FileDialog
    {
    public:
        static void Init();
        static void Shutdown();

        template <typename T>
        static bool OpenFileDialog(FilePath& outPath, StringView extensionName, const T& extensions)
        {
            DynamicArray<StringView> extensionsList;
            for (StringView extension : extensions)
            {
                extensionsList.push_back(extension);
            }

            return OpenFileDialog(outPath, extensionName, extensionsList);
        }

        template <typename T>
        static bool SaveFileDialog(FilePath& outPath, StringView extensionName, const T& extensions)
        {
            DynamicArray<StringView> extensionsList;
            for (StringView extension : extensions)
            {
                extensionsList.push_back(extension);
            }
            return SaveFileDialog(outPath, extensionName, extensionsList);
        }

        static bool OpenFileDialog(FilePath& outPath, StringView extensionName, const DynamicArray<StringView>& extensions);
        static bool SaveFileDialog(FilePath& outPath, StringView extensionName, const DynamicArray<StringView>& extensions);
    };
}
