#pragma once

#include <onyx/entity/entityregistry.h>

namespace Onyx::Graphics
{
    struct FrameContext;
}

namespace Onyx::Entity
{
    class ECSSystem
    {
    public:
        virtual ~ECSSystem() = default;

        void SetRegistry(EntityRegistry& registry) { m_Registry = &registry; }
        void Update(onyxU64 deltaTime, Graphics::FrameContext& context) { OnUpdate(deltaTime, context); }

    private:
        virtual void OnUpdate(onyxU64 /*deltaTime*/, Graphics::FrameContext& /*context*/) {}

    protected:
        EntityRegistry* m_Registry = nullptr;
    };
}
