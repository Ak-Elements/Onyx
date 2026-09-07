implementing lighting;

enum LightType : uint8_t { Directional, Point, Spot };

public static const uint32_t MaxLights = 64;
static const uint32_t MaxDirectionalLights = 8;

public static const uint32_t ClustersX = 16;
public static const uint32_t ClustersY = 9;
public static const uint32_t ClustersZ = 24;

public static const uint32_t UpdateClustersThreadsX = 16;
public static const uint32_t UpdateClustersThreadsY = 9;
public static const uint32_t UpdateClustersThreadsZ = 4;
public static const uint32_t UpdateClustersThreadsXYZ = UpdateClustersThreadsX * UpdateClustersThreadsY *
                                                        UpdateClustersThreadsZ;

static const float3 Dielectric = float3( 0.04 );
