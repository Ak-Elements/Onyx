#pragma once

#include <onyx/engine/enginesystem.h>

#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputactionsmap.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Input
{
    struct InputBindingContext;
    struct InputActionsMap;
    class InputActionsAsset;
    struct InputActionsContext;
    struct InputAction;
    class InputSystem;

    struct InputActionEvent
    {
        InputActionEvent(StringId64 id, void* data)
        : m_ActionId(id)
        , m_Data(data)
        {}

        template <typename T>
        const T& GetData() const { return *static_cast<T*>(m_Data); }

        StringId64 GetId() const { return m_ActionId; }

    private:
        StringId64 m_ActionId;
        void* m_Data = nullptr;
    };

    class InputActionSystem : public IEngineSystem
    {
        using InputActionSignalT = Signal<void(const InputActionEvent&)>;
    public:
        
        ~InputActionSystem() override;

        void Init(InputSystem& inputSystem, Assets::AssetSystem& assetSystem);
        void Shutdown(InputSystem& inputSystem);

        void Update(onyxU64 deltaTime);

        void SetActionsMapAsset(const Reference<InputActionsAsset>& inputAsset);
        void SetCurrentInputActionMap(StringId32 id);

        template<auto Candidate, typename... Type>
        void OnInput(StringId64 actionId, Type&&...value_or_instance)
        {
            auto it = std::find_if(m_CurrentActionStates.begin(), m_CurrentActionStates.end(), [&](const InputActionState& state)
            {
                return state.ActionId == actionId;
            });

            if (it == m_CurrentActionStates.end())
            {
                it = m_CurrentActionStates.emplace(m_CurrentActionStates.end(), actionId);
            }

            it->GetOnInputEvent().template Connect<Candidate>(std::forward<Type...>(value_or_instance...));
        }

        template <typename... Type>
        void Disconnect(Type&&...value_or_instance)
        {
            for (InputActionState& actionState : m_CurrentActionStates)
            {
                actionState.GetOnInputEvent().Disconnect(std::forward<Type...>(value_or_instance...));
            }
        }

    private:
        void InitContext();
        void UpdateContext(InputActionsMap& context);

    private:
        struct InputActionState
        {
            InputActionState(StringId64 actionId) : ActionId(actionId) {}

            Sink<InputActionSignalT> GetOnInputEvent() { return Sink<InputActionSignalT>(InputActionSignal); }
            InputActionSignalT InputActionSignal;

            DynamicArray<UniquePtr<InputBindingContext>> BindingContexts;
            StringId64 ActionId;
        };

        InputSystem* m_InputSystem;

        Reference<InputActionsAsset> m_InputActionsAsset;
        DynamicArray<InputActionState> m_CurrentActionStates;

        StringId32 m_ContextId = 0;
    };
}
