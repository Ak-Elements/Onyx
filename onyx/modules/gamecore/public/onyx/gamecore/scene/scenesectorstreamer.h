#pragma once
#include <onyx/gamecore/scene/scenesector.h>

namespace Onyx::GameCore
{
    class Scene;

    class SceneSectorStreamer
    {
        static constexpr onyxU32 SECTOR_SIZE = 16384;
        static constexpr onyxU32 SECTOR_SIZE_SQUARED = SECTOR_SIZE * SECTOR_SIZE;

        friend struct SceneSerializer;
    public:
        SceneSectorStreamer(Scene& scene);

        void Update(const Vector3f& loadCenter);

        void SetStreamInDistance(onyxF64 distance) { m_CloseStreamInDistance = distance; }
        void SetStreamOutDistance(onyxF64 distance) { m_CloseStreamOutDistance = std::max(distance, m_CloseStreamInDistance); }

        const DynamicArray<SceneSector>& GetSectors() const { return m_Sectors; }

        void AddEntity(Entity::EntityId entity);
        void RemoveEntity(Entity::EntityId entity);

    private:
        void LoadSectorEntity(const SceneSector& sector, SectorEntity& entity);
        void UnloadSectorEntity(SceneSector& sector, SectorEntity& entity);

    private:
        Scene* m_Scene;
        DynamicArray<SceneSector> m_Sectors;

        onyxF64 m_CloseStreamInDistance = 100.0f * 100.0f;
        onyxF64 m_CloseStreamOutDistance = 150.0f * 150.0f;
    };
}
