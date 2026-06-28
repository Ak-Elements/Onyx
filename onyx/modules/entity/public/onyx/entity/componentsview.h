#pragma once

#if ONYX_IS_EDITOR

#include <onyx/entity/entity.h>

#include <entt/entity/registry.hpp>

namespace onyx::ecs {

template < typename StorageIterableT >
class ComponentViewT {
  public:
    using StorageIterable = StorageIterableT;
    using StorageIterator = decltype( std::declval< StorageIterable& >().begin() );

    class Iterator {
      public:
        Iterator( StorageIterator it, StorageIterator end, ecs::EntityId entity )
            : m_it( it )
            , m_end( end )
            , m_entity( entity ) {
            skipToValid();
        }

        Iterator& operator++() {
            ++m_it;
            skipToValid();
            return *this;
        }

        bool operator!=( const Iterator& other ) const { return m_it != other.m_it; }

        auto operator*() const { return *m_it; }

      private:
        void skipToValid() {
            while( m_it != m_end && !m_it->second.contains( m_entity ) ) {
                ++m_it;
            }
        }

        StorageIterator m_it;
        StorageIterator m_end;
        ecs::EntityId m_entity;
    };

    ComponentViewT( StorageIterable storage, ecs::EntityId entity )
        : m_storage( std::move( storage ) )
        , m_entity( entity ) {}

    Iterator begin() { return Iterator( m_storage.begin(), m_storage.end(), m_entity ); }
    Iterator end() { return Iterator( m_storage.end(), m_storage.end(), m_entity ); }

  private:
    StorageIterable m_storage;
    ecs::EntityId m_entity;
};

} // namespace onyx::ecs

#endif
