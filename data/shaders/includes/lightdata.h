#include "common.h"

const int LightType_Directional = 0;
const int LightType_Point = 1;
const int LightType_Spot = 2;

const int MAX_LIGHTS = 64;

#define CLUSTERS_X 16
#define CLUSTERS_Y 8
#define CLUSTERS_Z 24

#define CLUSTER_THREADS_X 16
#define CLUSTER_THREADS_Y 8
#define CLUSTER_THREADS_Z 4

struct DirectionalLight
{
    vec3 Direction;
    float Intensity;

    vec3 Color;
    int ShadowSamples;

    bool IsShadowCasting;
    float Pad[7];// unused
};

struct PointLight
{
    vec3 Position;
    float Intensity;

    vec3 Color;
    float Radius;

    bool IsShadowCasting;
    float Pad[7];// unused
};

struct SpotLight
{
    vec3 Position;
    float Intensity;

    vec3 Direction;
    float AngleAttenuation;

    vec3 Color;
    float Range;

    float Angle;
    float Falloff;
    bool IsShadowCasting;
    float Pad;// unused
};

struct LightEnvironment
{
    DirectionalLight DirectionalLights[MAX_LIGHTS];
    PointLight PointLights[MAX_LIGHTS];
    SpotLight SpotLights[MAX_LIGHTS];

    int DirectionalLightCount;
    int PointLightCount;
    int SpotLightCount;
    int Padding;
};

struct LightGridCell
{
    uint PointLightOffset;
    uint PointLightCount;
    uint SpotLightOffset;
    uint SpotLightCount;
};

struct LightIndices
{
    uint PointLightIndex;
    uint SpotLightIndex;
};

// lights in the scene
layout(set = LIGHTING_SET, binding = 0) readonly buffer LightsBuffer
{
    LightEnvironment Lights;
} sbo_Lights[];

layout (std430, set = LIGHTING_SET, binding = 1) buffer LightIndexSSB
{
    LightIndices Indices[];
} sbo_GlobalLightIndices;

layout (std430, set = LIGHTING_SET, binding = 2) buffer LightGridSSBO
{
    LightGridCell Cells[];
} sbo_LightGrid;

#define SceneLights sbo_Lights[0].Lights