#pragma once

namespace onyx {
struct IEngineVariable {
    explicit IEngineVariable( StringId32 id );
    virtual ~IEngineVariable();

    template < typename T >
    T get() const {
        *( std::bit_cast< T* >( doGet() ) );
    }

    template < typename T >
    void set( T value ) {
        doSet( std::bit_cast< void* >( &value ) );
    }

    ONYX_NO_DISCARD virtual uint32_t getRuntimeTypeId() const = 0;

    ONYX_NO_DISCARD StringId32 getId() const { return m_id; }

  private:
    virtual void doSet( void* value ) = 0;
    ONYX_NO_DISCARD virtual void* doGet() const = 0;

  private:
    StringId32 m_id;
};

template < typename T > requires Numeric< T > || (!Numeric< T >) || Invokable< T >
struct EngineVariable : public IEngineVariable {
  public:
    explicit EngineVariable( StringId32 id )
        : IEngineVariable( id ) {}

    EngineVariable( StringId32 id, T defaultValue )
        : IEngineVariable( id )
        , m_value( defaultValue ) {}

    ONYX_NO_DISCARD uint32_t getRuntimeTypeId() const override { return TypeHash< EngineVariable< T > >(); }

    void set( T value ) { m_value = value; }
    T get() const { return m_value; }

  private:
    ONYX_NO_DISCARD void* doGet() const override { return std::bit_cast< void* >( &m_value ); }
    void doSet( void* value ) override { m_value = *std::bit_cast< T* >( value ); }

  private:
    T m_value;
};

template < Numeric T > requires std::is_arithmetic_v< T >
struct EngineVariable< T > : public IEngineVariable {
    using DataT = T;

  public:
    explicit EngineVariable( StringId32 id )
        : IEngineVariable( id ) {}

    EngineVariable( StringId32 id, T defaultValue )
        : IEngineVariable( id )
        , m_value( defaultValue ) {}

    EngineVariable( StringId32 id, T defaultValue, T minValue )
        : IEngineVariable( id )
        , m_value( defaultValue )
        , m_minValue( minValue ) {}

    EngineVariable( StringId32 id, T defaultValue, T minValue, T maxValue )
        : IEngineVariable( id )
        , m_value( defaultValue )
        , m_minValue( minValue )
        , m_maxValue( maxValue ) {}

    ONYX_NO_DISCARD uint32_t getRuntimeTypeId() const override { return TypeHash< EngineVariable< T > >(); }

    void set( T value ) { m_value = std::clamp( value, m_minValue, m_maxValue ); }
    T get() const { return m_value; }

    T getMin() const { return m_minValue; }
    T getMax() const { return m_maxValue; }

  private:
    ONYX_NO_DISCARD void* doGet() const override { return std::bit_cast< void* >( &m_value ); }
    void doSet( void* value ) override { m_value = *std::bit_cast< T* >( value ); }

  private:
    T m_value;
    T m_minValue = std::numeric_limits< T >::lowest();
    T m_maxValue = std::numeric_limits< T >::max();
};

template < Invokable T > requires Invokable< T >
struct EngineVariable< T > : public IEngineVariable {
    explicit EngineVariable( StringId32 id )
        : IEngineVariable( id ) {}

    EngineVariable( StringId32 id, T invokable )
        : IEngineVariable( id )
        , m_invokable( invokable ) {}

    ONYX_NO_DISCARD uint32_t getRuntimeTypeId() const override { return TypeHash< EngineVariable< T > >(); }

    void invoke() {
        if( m_invokable != nullptr )
            m_invokable();
    }

  private:
    ONYX_NO_DISCARD void* doGet() const override { ONYX_ASSERT( false, "Can not get invokable EngineVariable." ); }
    void doSet( void* value ) override { ONYX_ASSERT( false, "Can not set invokable EngineVariable." ); }

  private:
    T* m_invokable = nullptr;
};
} // namespace onyx
