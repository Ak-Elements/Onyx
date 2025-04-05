#include <onyx/filesystem/filedialog.h>

#include <nfd.h>
namespace Onyx::FileSystem
{
    void FileDialog::Init()
    {
        NFD_Init();
    }

    void FileDialog::Shutdown()
    {
        NFD_Quit();
    }

    bool FileDialog::OpenFileDialog(Filepath& outPath, StringView extensionName, const DynamicArray<StringView>& extensions)
    {
        nfdu8char_t* path;
        nfdopendialogu8args_t args = { 0 };

        DynamicArray<nfdu8filteritem_t> filters;
        for (StringView extension : extensions)
        {
            filters.emplace_back(extensionName.data(), extension.data());
        }

        const String workingDirectory = Path::GetWorkingDirectory().string();
        args.defaultPath = workingDirectory.c_str();
        args.filterList = filters.data();
        args.filterCount = static_cast<nfdfiltersize_t>(filters.size());
        nfdresult_t result = NFD_OpenDialogU8_With(&path, &args);

        if (result != NFD_OKAY)
        {
            return false;
        }

        outPath = Path::ConvertToMountPath(path);
        NFD_FreePathU8(path);
        return true;
    }

    bool FileDialog::SaveFileDialog(Filepath& outPath, StringView extensionName, const DynamicArray<StringView>& extensions)
    {
        nfdu8char_t* path;
        nfdsavedialogu8args_t args = { 0 };

        DynamicArray<nfdu8filteritem_t> filters;
        for (StringView extension : extensions)
        {
            filters.emplace_back(extensionName.data(), extension.data());
        }

        const String workingDirectory = Path::GetWorkingDirectory().string();
        args.defaultPath = workingDirectory.c_str();
        args.filterList = filters.data();
        args.filterCount = static_cast<nfdfiltersize_t>(filters.size());

        nfdresult_t result = NFD_SaveDialogU8_With(&path, &args);

        if (result != NFD_OKAY)
        {
            return false;
        }

        outPath = path;
        NFD_FreePathU8(path);
        return true;
    }
}
