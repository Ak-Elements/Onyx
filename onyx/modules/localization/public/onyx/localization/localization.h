#pragma once

#include <onyx/localization/localizedstring.h>

namespace onyx::localization
{
    class LocalizationModule;

    // Generic
    namespace generic
    {
#if ONYX_IS_EDITOR
        inline LocalizedString Name;
        inline LocalizedString Rename;
        inline LocalizedString None;
        inline LocalizedString Invalid;

        inline LocalizedString Add;
        inline LocalizedString Create;
        inline LocalizedString Delete;
        inline LocalizedString Duplicate;
        inline LocalizedString Search;

        inline LocalizedString Open;
        inline LocalizedString Save;
        inline LocalizedString SaveAs;

        inline LocalizedString File;
        inline LocalizedString Type;

        inline LocalizedString Default;
        inline LocalizedString Visibility;

#endif
    }

    void InitLocalization(const LocalizationModule& localizationModule);
}
