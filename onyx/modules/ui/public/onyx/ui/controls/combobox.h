#pragma once

namespace onyx::localization
{
    struct LocalizedString;
}

namespace onyx::ui
{
    bool BeginCombobox(StringView id, const localization::LocalizedString& label);
    bool BeginCombobox(StringView id, StringView label);
    void EndCombobox();
}
