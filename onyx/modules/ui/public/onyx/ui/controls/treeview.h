#pragma once

namespace Onyx::Ui
{
    enum class TreeViewFlags
    {
        None = 0,
        ForceOpenAll = 1 << 0,
        ForceCloseAll = 1 << 1,
    };

    struct TreeItem
    {
        String Label;
        std::function<void()> OnSelected;

        HashMap<String, TreeItem> Children;
    };

    bool RenderTreeView(StringView id, const TreeItem& root);
    bool RenderTreeView(StringView id, const TreeItem& root, TreeViewFlags flags);
}