#include <onyx/input/inputactionsmap.h>

#include <onyx/input/inputaction.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Input
{
    InputActionsMap::InputActionsMap(StringId32 id, const StringView& name, const FileSystem::JsonValue& json)
        : m_Id(id)
        , m_Name(name)
    {
        for (const nlohmann::basic_json<nlohmann::ordered_map>& inputActionJsonObj : json.Json)
        {
            FileSystem::JsonValue inputActionJson { inputActionJsonObj };

            InputAction action;
            if (InputAction::FromJson(inputActionJson, action) == false)
            {
                continue;
            }

            if (HasInputAction(action.GetId()))
            {
                ONYX_LOG_ERROR("Input action with the same ID is already registered. Skipping input action {}", action.GetId());
                continue;
            }

            m_Actions.push_back(std::move(action));
        }
    }

    bool InputActionsMap::HasInputAction(StringId64 actionId) const
    {
        for (const InputAction& action : m_Actions)
        {
            if (action.GetId() == actionId)
                return true;
        }

        return false;
        
    }
}
