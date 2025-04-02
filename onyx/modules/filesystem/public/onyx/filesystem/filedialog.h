#pragma once

#include <onyx/typetraits.h>
#include <onyx/filesystem/path.h>

namespace Onyx::FileSystem
{
    class FileDialog
    {
    public:
        static void Init();
        static void Shutdown();

        template <typename T> requires is_tuple_v<T>
        static bool OpenFileDialog(Filepath& outPath, StringView extensionName, const T& extensionsTuple)
        {
            DynamicArray<StringView> extensions;
            std::apply([&](auto&&... extension)
                {
                    (extensions.emplace_back(extension),
                        ...);
                },
                extensionsTuple);

            return OpenFileDialog(outPath, extensionName, extensions);
        }

        template <typename T> requires is_tuple_v<T>
        static bool SaveFileDialog(Filepath& outPath, StringView extensionName, const T& extensionsTuple)
        {
            DynamicArray<StringView> extensions;
            std::apply([&](auto&&... extension)
                {
                    (extensions.emplace_back(extension),
                        ...);
                },
                extensionsTuple);

            return SaveFileDialog(outPath, extensionName, extensions);
        }

        static bool OpenFileDialog(Filepath& outPath, StringView extensionName, const DynamicArray<StringView>& extensions);
        static bool SaveFileDialog(Filepath& outPath, StringView extensionName, const DynamicArray<StringView>& extensions);
    };
}
