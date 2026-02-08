#pragma once

namespace Onyx
{
    struct IEngineVariable
    {
        IEngineVariable(StringId32 id);
        virtual ~IEngineVariable();

        template <typename T>
        T Get() const { *(std::bit_cast<T*>(DoGet())); }

        template <typename T>
        void Set(T value) { DoSet(std::bit_cast<void*>(&value)); }
        
        virtual onyxU32 GetRuntimeTypeId() const = 0;

        StringId32 GetId() const { return m_Id; }

    private:
        virtual void DoSet(void* value) = 0;
        virtual void* DoGet() const = 0; 

    private:
        StringId32 m_Id;
    };

    template <typename T> requires 
        Numeric<T> ||
        (!Numeric<T>) ||
        Invokable<T>
    struct EngineVariable : public IEngineVariable
    {
    public:
        EngineVariable(StringId32 id)
         : IEngineVariable(id)
        {
        }

        EngineVariable(StringId32 id, T defaultValue)
            : IEngineVariable(id)
            , m_Value(defaultValue)
        {
        }
        
        onyxU32 GetRuntimeTypeId() const override { return TypeHash<EngineVariable<T>>(); }

        void Set(T value) { m_Value = value; }
        T Get() const { return m_Value; }

    private:
        void* DoGet() const override { return std::bit_cast<void*>(&m_Value); }
        void DoSet(void *value) override { m_Value = *std::bit_cast<T*>(value); }

    private:
        T m_Value;
    };

    template <Numeric T> requires std::is_arithmetic_v<T>
    struct EngineVariable<T> : public IEngineVariable
    {
        using DataT = T;
    public:
        EngineVariable(StringId32 id)
         : IEngineVariable(id)
        {
        }

        EngineVariable(StringId32 id, T defaultValue)
            : IEngineVariable(id)
            , m_Value(defaultValue)
        {
        }
        
        EngineVariable(StringId32 id, T defaultValue, T minValue)
            : IEngineVariable(id)
            , m_Value(defaultValue)
            , m_MinValue(minValue)
        {
        }

        EngineVariable(StringId32 id, T defaultValue, T minValue, T maxValue)
            : IEngineVariable(id)
            , m_Value(defaultValue)
            , m_MinValue(minValue)
            , m_MaxValue(maxValue)
        {
        }

        onyxU32 GetRuntimeTypeId() const override { return TypeHash<EngineVariable<T>>(); }

        void Set(T value) { m_Value = std::clamp(value, m_MinValue, m_MaxValue); }
        T Get() const { return m_Value; }

        T GetMin() const { return m_MinValue; }
        T GetMax() const { return m_MaxValue; }

    private:
        void* DoGet() const override { return std::bit_cast<void*>(&m_Value); }
        void DoSet(void *value) override { m_Value = *std::bit_cast<T*>(value); }

    private:
        T m_Value;
        T m_MinValue = std::numeric_limits<T>::lowest();
        T m_MaxValue = std::numeric_limits<T>::max();
    };

    template <Invokable T> requires Invokable<T>
    struct EngineVariable<T> : public IEngineVariable
    {
        EngineVariable(StringId32 id)
         : IEngineVariable(id)
        {
        }

        EngineVariable(StringId32 id, T invokable)
            : IEngineVariable(id)
            , m_Invokable(invokable)
        {
        }
    
        onyxU32 GetRuntimeTypeId() const override { return TypeHash<EngineVariable<T>>(); }

        void Invoke() 
        {
            if (m_Invokable != nullptr)
                m_Invokable();
        }

    private:
        void* DoGet() const override { ONYX_ASSERT(false, "Can not get invokable EngineVariable."); }
        void DoSet(void *value) override { ONYX_ASSERT(false, "Can not set invokable EngineVariable."); }

    private:
        T* m_Invokable = nullptr;
    };
}