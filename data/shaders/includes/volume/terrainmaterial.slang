struct TerrainMaterial {
    uint32_t BaseColor;
    uint32_t Normal;
    uint32_t Height;
}

struct TerrainMaterialDatabase {
    TerrainMaterial Materials[ ];
}
