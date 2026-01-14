#pragma once

#if !ONYX_IS_RETAIL
namespace Onyx::Ui
{
    template <typename T>
    struct PropertyInspector;

    template <typename T>
    concept HasPropertyInspector = requires(T& instance, bool forceShow)
    {
        { PropertyInspector<T>::Draw(instance, forceShow) } -> std::same_as<bool>;
    };

    template <typename T>
    struct PropertyInspector
    {
        static bool Draw(T& /*instance*/, bool /*forceShow*/) { return false; }
    };

    struct PropertyInspectors
    {
    public:
        template <typename T> requires HasPropertyInspector<T>
        static void Register()
        {
            constexpr onyxU32 typeId = TypeHash<T>();
            s_PropertyInspectors[typeId] = [](void* instance, bool forceShow)
            {
                return PropertyInspector<T>::Draw(*static_cast<T*>(instance), forceShow);
            };
        }

        static bool Draw(onyxU32 runtimeTypeId, void* instance, bool forceShow)
        {
            auto it = s_PropertyInspectors.find(runtimeTypeId);
            if (it == s_PropertyInspectors.end())
            {
                ONYX_LOG_WARNING("Type {} is missing an inspector. Should it be marked as runtime only?", runtimeTypeId);
                return false;
            }

            return it->second(instance, forceShow);
        }

    private:
        static inline HashMap<onyxU32, bool(*)(void*, bool)> s_PropertyInspectors;
    };
}
#endif