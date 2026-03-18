#pragma once

#include <onyx/engine/enginesystem.h>
#include <onyx/assets/assethandle.h>

#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/inputactions/inputactionsmap.h>

namespace onyx::assets
{
    class AssetSystem;
}

namespace onyx::input
{
    class InputSystem;
}

namespace onyx::input_actions
{
    struct InputBindingContext;
    struct InputActionsMap;
    struct InputAction;

    struct InputActionEvent
    {
        InputActionEvent(StringId64 id, Vector3f32 value)
        : m_ActionId(id)
        , m_Value(value)
        {}

        template <typename T>
        T GetData() const
        {
            if constexpr (std::is_same_v<bool, T>)
            {
                return m_Value[0] != 0.0f;
            }
            else if constexpr (std::is_same_v<onyxF32, T>)
            {
                return m_Value[0];
            }
            else if constexpr (std::is_same_v<Vector2f32, T>)
            {
                return Vector2f32(m_Value);
            }
            else
            {
                return m_Value;
            }
        }


        StringId64 GetId() const { return m_ActionId; }

    private:
        StringId64 m_ActionId;
        Vector3f32 m_Value;
    };

    struct InputActionState
    {
        InputActionState(StringId64 actionId) : ActionId(actionId) {}

        Vector3f32 Value;
        StringId64 ActionId;
    };

    struct InputActionSystemSettings
    {
        assets::AssetId InputActionId { "engine:/inputcontexts.oinput" };
    };

    class InputActionSystem : public IEngineSystem
    {
        friend struct Serialization<InputActionSystem>;

        using InputActionSignalT = Signal<void(const InputActionEvent&)>;
    public:
        static constexpr StringId32 TypeId = "onyx::input_actions::InputActionSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        InputActionSystem(const InputActionSystemSettings& settings, input::InputSystem& inputSystem, assets::AssetSystem& assetSystem);
        ~InputActionSystem() override;

        void Update();

        void SetActionsMapAsset(assets::AssetHandle<InputActionsContext> inputAsset);
        void SetCurrentInputActionMap(StringId32 id);

        Optional<InputActionState*> GetActionState(StringId64 actionId);
        Optional<const InputActionState*> GetActionState(StringId64 actionId) const;

        template<auto Candidate, typename... Type>
        void OnInput(StringId64 actionId, Type&&... value_or_instance)
        {
            Sink sink(m_InputActionSignals[actionId]);
            sink.template Connect<Candidate>(std::forward<Type...>(value_or_instance)...);
        }

        template <typename... Type>
        void Disconnect(Type&&...value_or_instance)
        {
            for (InputActionSignalT& inputSignal : m_InputActionSignals | std::views::values)
            {
                Sink sink(inputSignal);
                sink.Disconnect(std::forward<Type...>(value_or_instance)...);
            }
        }

        bool IsActionTriggered(StringId64 actionId) const;

    private:
        void InitContext();
        void UpdateContext(InputActionsMap& context);

    private:
        input::InputSystem* m_InputSystem = nullptr;

        assets::AssetHandle<InputActionsContext> m_InputActionsAsset;
        DynamicArray<InputActionState> m_CurrentActionStates;
        HashMap<StringId64, InputActionSignalT> m_InputActionSignals;

        StringId32 m_ContextId = 0;
    };
}

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputActionSystemSettings>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputActionSystemSettings& settings);
        static bool Deserialize(const Deserializer& deserializer, input_actions::InputActionSystemSettings& outSettings);
    };
}
