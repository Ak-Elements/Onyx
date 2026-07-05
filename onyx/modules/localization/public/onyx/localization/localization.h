#pragma once

#include <onyx/localization/localizedstring.h>

namespace onyx::localization {
class LocalizationModule;

// Generic
namespace generic {
#if ONYX_IS_EDITOR
inline LocalizedString Name;
inline LocalizedString Rename;
inline LocalizedString None;
inline LocalizedString Invalid;
inline LocalizedString MoveUp;
inline LocalizedString MoveDown;
inline LocalizedString InsertBefore;
inline LocalizedString InsertAfter;

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

inline LocalizedString AddCollectionEntry;
inline LocalizedString CollectionEntry;
inline LocalizedString CollectionSize;
#endif
} // namespace generic

void InitLocalization( const LocalizationModule& localizationModule );
} // namespace onyx::localization
