#pragma once

namespace onyx::ui {
enum class TreeViewFlags : uint8_t {
    None = 0,
    ForceOpenAll = 1 << 0,
    ForceCloseAll = 1 << 1,
};

struct TreeItem {
    String Label;
    std::function< void() > OnSelected;

    HashMap< String, TreeItem > Children;
};

bool renderTreeView( StringView id, const TreeItem& root );
bool renderTreeView( StringView id, const TreeItem& root, TreeViewFlags flags );
} // namespace onyx::ui
