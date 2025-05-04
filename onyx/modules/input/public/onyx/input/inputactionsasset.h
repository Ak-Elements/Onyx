#pragma once

#include <onyx/assets/asset.h>
#include <onyx/input/inputactionsmap.h>

namespace Onyx::Input
{
    class InputActionsAsset : public Assets::Asset<InputActionsAsset>
    {
    public:
        static constexpr StringId32 TypeId{ "Onyx::Input::Assets::TextureAsset" };
        StringId32 GetTypeId() const { return TypeId; }

        HashMap<StringId32, InputActionsMap>& GetMaps() { return m_Contexts; }
        const HashMap<StringId32, InputActionsMap>& GetMaps() const { return m_Contexts; }

        bool HasContext(StringId32 id) const { return m_Contexts.contains(id); }

        InputActionsMap& GetContext(StringId32 id) { ONYX_ASSERT(HasContext(id)); return m_Contexts.at(id); }
        const InputActionsMap& GetContext(StringId32 id) const { ONYX_ASSERT(HasContext(id)); return m_Contexts.at(id); }

        void SetName(const String& name) { m_Name = name; }
        const String& GetName() const { return m_Name; }

    private:
        String m_Name;
        HashMap<StringId32, InputActionsMap> m_Contexts;
    };
}
