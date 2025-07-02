#pragma once

namespace Onyx::Localization
{
    struct LocalizedString;
}

namespace Onyx::Ui
{
    bool BeginCombobox(StringView id, const Localization::LocalizedString& label);
    bool BeginCombobox(StringView id, StringView label);
    void EndCombobox();
}
