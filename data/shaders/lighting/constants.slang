implementing lighting;

enum LightType : uint8_t { Directional, Point, Spot };

static const uint32_t MaxLights = 64;
static const uint32_t MaxDirectionalLights = 8;

public static const uint8_t ClustersX = 16;
public static const uint8_t ClustersY = 9;
public static const uint8_t ClustersZ = 24;

static const float3 Dielectric = float3( 0.04 );
