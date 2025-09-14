#pragma once

namespace Onyx::Ui
{
    enum class TreeViewFlags
    {
        None = 0,
        ForceOpenAll = 1,
        ForceCloseAll = 2,
    };
    ONYX_ENABLE_BITMASK_OPERATORS(TreeViewFlags);

    struct TreeItem
    {
        String Label;
        InplaceFunction<void()> OnSelected;

        HashMap<String, TreeItem> Children;
    };

    void RenderTreeView(StringView id, const TreeItem& root);
    void RenderTreeView(StringView id, const TreeItem& root, TreeViewFlags flags);
}