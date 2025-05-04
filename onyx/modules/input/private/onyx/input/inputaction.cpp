#include <onyx/input/inputaction.h>
#include <onyx/input/inputbinding.h>
#include <onyx/input/inputbindingsregistry.h>

namespace Onyx::Input
{
    InputAction::InputAction() = default;

    InputAction::InputAction(StringId64 actionId)
        : m_Id(actionId)
    {
    }


#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void InputAction::SetName(StringView name)
    {
        m_Id = StringId64(name);
    }
#endif


    bool InputAction::FromJson(const FileSystem::JsonValue& json, InputAction& outAction)
    {
        StringId64 actionId;
        json.Get("id", actionId);


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
        for (const auto& bindingJson : bindings.Json)
        {
            FileSystem::JsonValue bindingJsonObj{ bindingJson };
            StringId32 bindingTypeId;
            bindingJsonObj.Get("typeId", bindingTypeId);

            UniquePtr<InputBinding> binding = InputBindingsRegistry::CreateBinding(bindingTypeId);

            InputType type;
            bindingJsonObj.Get("inputType", type);
            binding->SetInputType(type);

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
