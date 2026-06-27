#pragma once

#include <onyx/assets/asset.h>
#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::input_actions {
class InputActionsContext : public assets::Asset< InputActionsContext > {
  public:
    static constexpr StringId32 TypeId{ "onyx::input_actions::InputActionsContext" };
    static StringId32 getTypeId() { return TypeId; }

    HashMap< StringId32, InputActionsMap >& getMaps() { return m_contexts; }
    const HashMap< StringId32, InputActionsMap >& getMaps() const { return m_contexts; }

    bool hasContext( StringId32 id ) const { return m_contexts.contains( id ); }

    InputActionsMap& getContext( StringId32 id ) {
        ONYX_ASSERT( hasContext( id ) );
        return m_contexts.at( id );
    }
    const InputActionsMap& getContext( StringId32 id ) const {
        ONYX_ASSERT( hasContext( id ) );
        return m_contexts.at( id );
    }

    void setName( const String& name ) { m_name = name; }
    const String& getName() const { return m_name; }

  private:
    String m_name;
    HashMap< StringId32, InputActionsMap > m_contexts;
};
} // namespace onyx::input_actions
