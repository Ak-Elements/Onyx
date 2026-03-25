#pragma once
#include <onyx/gamecore/scene/scenesector.h>

namespace onyx::ecs {
enum class EntityId : uint32_t;
}

namespace onyx::game_core {
class Scene;

class SceneSectorStreamer {
    static constexpr uint32_t SECTOR_SIZE = 16384;
    static constexpr uint32_t SECTOR_SIZE_SQUARED = SECTOR_SIZE * SECTOR_SIZE;

    friend struct SceneSerializer;

  public:
    SceneSectorStreamer( Scene& scene );

    void Update( const Vector3f32& loadCenter );

    void SetStreamInDistance( float64 distance ) { m_CloseStreamInDistance = distance; }
    void SetStreamOutDistance( float64 distance ) {
        m_CloseStreamOutDistance = std::max( distance, m_CloseStreamInDistance );
    }

    const DynamicArray< SceneSector >& GetSectors() const { return m_Sectors; }

    void AddEntity( ecs::EntityId entity );
    void RemoveEntity( ecs::EntityId entity );

  private:
    void LoadSectorEntity( const SceneSector& sector, SectorEntity& entity );
    void UnloadSectorEntity( SceneSector& sector, SectorEntity& entity );

  private:
    Scene* m_Scene;
    DynamicArray< SceneSector > m_Sectors;

    float64 m_CloseStreamInDistance = 100.0f * 100.0f;
    float64 m_CloseStreamOutDistance = 150.0f * 150.0f;
};
} // namespace onyx::game_core
