#pragma once

namespace onyx::serialization::text::internal {
struct Scope {
    StringView::size_type NameStart = 0;
    StringView::size_type NameLength = 0;

    StringView::size_type DataStart = 0;
    StringView::size_type DataLength = 0;

    bool IsLeaf = false;
    DynamicArray< Scope > Children;
};

void initializeScope( StringView data, Scope& scope );
void updateScopeRange( Scope& scope, int64_t delta );
} // namespace onyx::serialization::text::internal
