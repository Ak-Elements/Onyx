#include <onyx/input/inputaction.h>
#include <onyx/input/inputbinding.h>
#include <onyx/input/inputbindingsregistry.h>

namespace Onyx::Input
{
    InputAction::InputAction() = default;

#if ONYX_IS_DEBUG
    InputAction::InputAction(onyxU64 actionId, const StringView& actionName)
        : m_Id(actionId)
        , m_Name(actionName)
    {
    }
#endif

    InputAction::InputAction(const StringView& actionName)
        : m_Id(Hash::FNV1aHash64(actionName))
#if ONYX_IS_DEBUG | ONYX_IS_EDITOR
        , m_Name(actionName)
#endif
    {
    }

    InputAction::InputAction(onyxU64 actionId)
        : m_Id(actionId)
    {
    }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void InputAction::SetName(StringView name)
    {
        m_Id = Hash::FNV1aHash64(name);
        m_Name = name;
    }
#endif


    bool InputAction::FromJson(const FileSystem::JsonValue& json, InputAction& outAction)
    {
        onyxU64 actionId;
        json.Get("id", actionId);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR

        StringView actionName;
        json.Get("name", actionName);
        const onyxU64 actionIdFromName = Hash::FNV1aHash64(actionName);

        if (actionId != actionIdFromName)
        {
            ONYX_LOG_ERROR("Input action id does not match the action name. Did the hash function fail or change?");
            return false;
        }

        outAction.m_Name = actionName;
#endif

        outAction.m_Id = actionId;

        json.Get("type", outAction.m_Type);

        if (outAction.m_Type == ActionType::Invalid)
        {
            ONYX_LOG_ERROR("Input action is missing action type.");
            return false;   
        }

        FileSystem::JsonValue bindings;
        if (json.Get("bindings", bindings) == false)
        {
            ONYX_LOG_ERROR("Input action is missing bindings.");
            return false;
        }

        onyxU32 boundInput;
        onyxU32 bindingClassId;
        for (const auto& bindingJson : bindings.Json)
        {
            FileSystem::JsonValue bindingJsonObj{ bindingJson };
            bindingJsonObj.Get("id", bindingClassId);

            UniquePtr<InputBinding> binding = InputBindingsRegistry::CreateBinding(bindingClassId);

            InputType type;
            bindingJsonObj.Get("type", type);
            binding->SetType(type);

            onyxU32 bindingSlotsCount = binding->GetInputBindingSlotsCount();
            for (onyxU32 i = 0; i < bindingSlotsCount; ++i)
            {
                StringView bindingSlotName = binding->GetInputBindingSlotName(i);
                bindingJsonObj.Get(bindingSlotName.empty() ? "input" : bindingSlotName, boundInput);
                binding->SetInputBindingSlot(i, boundInput);
            }

            outAction.m_Bindings.push_back(std::move(binding));
        }

        return true;
    }
}
