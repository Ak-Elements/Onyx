#include <onyx/input/inputactionsystem.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/input/inputaction.h>
#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputsystem.h>
#include <onyx/input/inputactionsmap.h>
#include <onyx/input/inputactionsserializer.h>
#include <onyx/input/inputbinding.h>
#include <onyx/input/inputbindingsregistry.h>

namespace Onyx::Input
{
    void InputActionSystem::Init(InputSystem& inputSystem, Assets::AssetSystem& assetSystem)
    {
        m_InputSystem = &inputSystem;

        Assets::AssetSystem::Register<InputActionsAsset, InputActionsSerializer>(assetSystem);

        InputBindingsRegistry::Register<InputBindingBool>(ActionType::Bool, "Bool");
        InputBindingsRegistry::Register<InputBindingAxis1D>(ActionType::Axis1D, "Axis 1D");
        InputBindingsRegistry::Register<InputBindingAxis1DComposite>(ActionType::Axis1D, "Axis 1D Composite");
        InputBindingsRegistry::Register<InputBindingAxis2D>(ActionType::Axis2D, "Axis 2D");
        InputBindingsRegistry::Register<InputBindingAxis2DComposite>(ActionType::Axis2D, "Axis 2D Composite");
        InputBindingsRegistry::Register<InputBindingAxis3D>(ActionType::Axis3D, "Axis 3D");
        InputBindingsRegistry::Register<InputBindingAxis3DComposite>(ActionType::Axis3D, "Axis 3D Composite");
    }

    InputActionSystem::~InputActionSystem() = default;

    void InputActionSystem::Update(GameTime /*deltaTime*/)
    {
        if (m_ContextId.IsValid() == false)
            return;

        UpdateContext(m_InputActionsAsset->GetContext(m_ContextId));
    }

    void InputActionSystem::SetActionsMapAsset(const Reference<InputActionsAsset>& inputAsset)
    {
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
            // TODO: we should only clear actions that are not present anymore
            //m_CurrentActionStates.clear();

            m_ContextId = id;

            if (id.IsValid() && m_InputActionsAsset.IsValid() && m_InputActionsAsset->IsLoaded())
            {
                InitContext();
            }
        }
    }

    void InputActionSystem::InitContext()
    {
        const InputActionsMap& context = m_InputActionsAsset->GetContext(m_ContextId);

        m_CurrentActionStates.reserve(context.GetActions().size());
        for (const InputAction& action : context.GetActions())
        {
            const DynamicArray<UniquePtr<InputBinding>>& bindings = action.GetBindings();

            auto it = std::find_if(m_CurrentActionStates.begin(), m_CurrentActionStates.end(), [&](const InputActionState& state)
                {
                    return state.ActionId == action.GetId();
                });

            InputActionState& actionState = (it == m_CurrentActionStates.end()) ? m_CurrentActionStates.emplace_back(action.GetId()) : *it;
            actionState.BindingContexts.clear();
            actionState.BindingContexts.reserve(bindings.size());

            for (const UniquePtr<InputBinding>& binding : bindings)
            {
                actionState.BindingContexts.emplace_back(binding->CreateContext());
            }
        }
    }

    void InputActionSystem::UpdateContext(InputActionsMap& context)
    {
        const DynamicArray<InputAction>& actions = context.GetActions();
        const onyxU32 actionsCount = static_cast<onyxU32>(actions.size());

        for (onyxU32 actionIndex = 0; actionIndex < actionsCount; ++actionIndex)
        {
            const InputAction& action = actions[actionIndex];

            auto it = std::ranges::find_if(m_CurrentActionStates, [&](const InputActionState& state)
                {
                    return state.ActionId == action.GetId();
                });

            if (it == m_CurrentActionStates.end())
                continue;

            InputActionState& actionState = *it;

            ONYX_ASSERT(actionState.ActionId == action.GetId());

            const DynamicArray<UniquePtr<InputBinding>>& bindings = action.GetBindings();
            const onyxU32 bindingsCount = static_cast<onyxU32>(bindings.size());
            for (onyxU32 bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex )
            {
                const UniquePtr<InputBinding>& binding = bindings[bindingIndex];
                UniquePtr<InputBindingContext>& bindingState = actionState.BindingContexts[bindingIndex];
                if (binding->UpdateBinding(*m_InputSystem, bindingState))
                {
                    InputActionEvent event { action.GetId(), bindingState->GetData() };
                    actionState.InputActionSignal.Dispatch(event);
                }
            }
        }
    }

    void InputActionSystem::Shutdown(InputSystem& /*inputSystem*/)
    {
    }
    
}
