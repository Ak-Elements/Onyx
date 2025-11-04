#include <onyx/input/inputactionsystem.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/input/inputaction.h>
#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputsystem.h>
#include <onyx/input/inputactionsmap.h>
#include <onyx/input/inputactionsserializer.h>
#include <onyx/input/bindings/inputbinding.h>

#include <onyx/input/bindings/inputbindingaxis1d.h>
#include <onyx/input/bindings/inputbindingaxis1dcomposite.h>
#include <onyx/input/bindings/inputbindingaxis2d.h>
#include <onyx/input/bindings/inputbindingaxis2dcomposite.h>
#include <onyx/input/bindings/inputbindingaxis3d.h>
#include <onyx/input/bindings/inputbindingaxis3dcomposite.h>
#include <onyx/input/bindings/inputbindingbool.h>
#include <onyx/input/bindings/inputbindingsfactory.h>
#include <onyx/input/triggers/dependantactiontrigger.h>
#include <onyx/input/triggers/inputtriggersfactory.h>
#include <onyx/serialize/deserializer.h>


namespace Onyx::Input
{
    void InputActionSystem::Init(InputSystem& inputSystem, Assets::AssetSystem& assetSystem)
    {
        m_InputSystem = &inputSystem;

        Assets::AssetSystem::Register<InputActionsAsset>();
        Assets::AssetSystem::Register<InputActionsSerializer>(assetSystem);

        InputBindingsFactory::Register<InputBindingBool>();
        InputBindingsFactory::Register<InputBindingAxis1D>();
        InputBindingsFactory::Register<InputBindingAxis1DComposite>();
        InputBindingsFactory::Register<InputBindingAxis2D>();
        InputBindingsFactory::Register<InputBindingAxis2DComposite>();
        InputBindingsFactory::Register<InputBindingAxis3D>();
        InputBindingsFactory::Register<InputBindingAxis3DComposite>();

        InputTriggersFactory::Register<DependantActionInputTrigger>();

        //m_ModifiersRegistry.Register<NegateInputModifier>();
        //m_ModifiersRegistry.Register<SwizzleInputModifier>();

        Reference<InputActionsAsset> defaultInputActionsMap;
        assetSystem.GetAsset(m_InputActionId, defaultInputActionsMap);
        defaultInputActionsMap->GetOnLoadedEvent().Connect<&InputActionSystem::SetActionsMapAsset>(this);
        if (defaultInputActionsMap.IsValid() && defaultInputActionsMap->IsLoaded())
        {
            SetActionsMapAsset(defaultInputActionsMap);
        }
    }

    InputActionSystem::~InputActionSystem() = default;

    void InputActionSystem::Update()
    {
        if (m_ContextId.IsValid() == false)
            return;

        UpdateContext(m_InputActionsAsset->GetContext(m_ContextId));
    }

    void InputActionSystem::SetActionsMapAsset(Reference<InputActionsAsset>& inputAsset)
    {
        inputAsset->GetOnLoadedEvent().Disconnect(this);

        if (m_InputActionsAsset != inputAsset)
        {
            StringId32 newContextId = m_ContextId;
            m_InputActionsAsset = inputAsset;
            m_ContextId = 0;

            if (m_InputActionsAsset && (m_InputActionsAsset->GetMaps().empty() == false))
            {
                if ((newContextId.IsValid() == false) || (m_InputActionsAsset->HasContext(newContextId) == false))
                {
                    newContextId = m_InputActionsAsset->GetMaps().begin()->first;
                }

                SetCurrentInputActionMap(newContextId);
            }
            else
            {
                SetCurrentInputActionMap(0);
            }

        }
    }

    void InputActionSystem::SetCurrentInputActionMap(StringId32 id)
    {
        if (id != m_ContextId)
        {
            m_CurrentActionStates.clear();

            // should we clear input signals not in the map anymore?

            m_ContextId = id;

            if (id.IsValid() && m_InputActionsAsset.IsValid())
            {
                InitContext();
            }
        }
    }

    Optional<const InputActionState*> InputActionSystem::GetActionState(StringId64 actionId) const
    {
        auto it = std::find_if(m_CurrentActionStates.begin(), m_CurrentActionStates.end(), [&](const InputActionState& state)
            {
                return state.ActionId == actionId;
            });

        if (it == m_CurrentActionStates.end())
            return std::nullopt;

        const InputActionState& actionState = *it;
        return &actionState;
    }

    Optional<InputActionState*> InputActionSystem::GetActionState(StringId64 actionId)
    {
        auto it = std::find_if(m_CurrentActionStates.begin(), m_CurrentActionStates.end(), [&](const InputActionState& state)
            {
                return state.ActionId == actionId;
            });

        if (it == m_CurrentActionStates.end())
            return std::nullopt;

        InputActionState& actionState = *it;
        return &actionState;
    }

    bool InputActionSystem::IsActionTriggered(StringId64 actionId) const
    {
        if (const InputActionState* state = GetActionState(actionId).value_or(nullptr))
        {
            return IsZero(state->Value) == false;
        }

        return false;
    }

    void InputActionSystem::InitContext()
    {
        const InputActionsMap& context = m_InputActionsAsset->GetContext(m_ContextId);

        m_CurrentActionStates.reserve(context.GetActions().size());
        for (const InputAction& action : context.GetActions())
        {
            auto it = std::find_if(m_CurrentActionStates.begin(), m_CurrentActionStates.end(), [&](const InputActionState& state)
                {
                    return state.ActionId == action.GetId();
                });

            if (it != m_CurrentActionStates.end())
                continue;

            m_CurrentActionStates.emplace_back(action.GetId());
        }
    }

    void InputActionSystem::UpdateContext(InputActionsMap& context)
    {
        DynamicArray<InputAction>& actions = context.GetActions();
        const onyxU32 actionsCount = static_cast<onyxU32>(actions.size());

        for (onyxU32 actionIndex = 0; actionIndex < actionsCount; ++actionIndex)
        {
            InputAction& action = actions[actionIndex];
            StringId64 actionId = action.GetId();

            Optional<InputActionState*> optionalActionState = GetActionState(actionId);
            ONYX_ASSERT(optionalActionState.has_value());

            InputActionState& actionState = *optionalActionState.value();

            DynamicArray<UniquePtr<InputBinding>>& bindings = action.GetBindings();
            const onyxU32 bindingsCount = static_cast<onyxU32>(bindings.size());

            bool hasTriggered = false;
            Vector3f32 newInputValue(std::numeric_limits<onyxF32>::lowest());
            for (onyxU32 bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex )
            {
                InputBinding& binding = *bindings[bindingIndex];

                Vector3f32 bindingInputValue;
                bool isTriggered = binding.Update(*m_InputSystem, *this, bindingInputValue);
               
                if (isTriggered)
                {
                    hasTriggered = true;
                    newInputValue.X = std::max(newInputValue.X, bindingInputValue.X);
                    newInputValue.Y = std::max(newInputValue.Y, bindingInputValue.Y);
                    newInputValue.Z = std::max(newInputValue.Z, bindingInputValue.Z);
                }
            }

            if (hasTriggered == false)
            {
                newInputValue = Vector3f32::Zero();
            }

            if (actionState.Value != newInputValue)
            {
                actionState.Value = newInputValue;
                InputActionEvent event{ actionId, actionState.Value };
                m_InputActionSignals[actionId].Dispatch(event);
            }
        }
    }

    void InputActionSystem::Shutdown(InputSystem& /*inputSystem*/)
    {
    }
}

namespace Onyx
{
    bool Serialization<Input::InputActionSystem>::Serialize(Serializer& /*serializer*/, const Input::InputActionSystem& /*system*/)
    {
        return true;
    }

    bool Serialization<Input::InputActionSystem>::Deserialize(const Deserializer& deserializer, Input::InputActionSystem& outSystem)
    {
        StringView inputMapPath;
        if (deserializer.Read<"inputmap">(inputMapPath))
        {
            outSystem.m_InputActionId = Assets::AssetId(FileSystem::Filepath(inputMapPath));
        }

        return true;
    }
}

