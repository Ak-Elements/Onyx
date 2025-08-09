

float GetClosetDistanceToCube(vec3 worldPosition, vec3 cubeCenter, vec3 halfExtents)
{
    float x = max(worldPosition.x - cubeCenter.x - halfExtents.x,
        cubeCenter.x - worldPosition.x - halfExtents.x);

    float y = max(worldPosition.y - cubeCenter.y - halfExtents.y,
        cubeCenter.y - worldPosition.y - halfExtents.y);

    float z = max(worldPosition.z - cubeCenter.z - halfExtents.z,
        cubeCenter.z - worldPosition.z - halfExtents.z);
    
    float d = max(x, max(y, z));
    return -d;
}

vec4 GetCubeValueAndGradient(vec3 worldPosition, vec3 cubeCenter, vec3 halfExtents)
{
    const float Epsilon = 0.0001;
    const float difference = Epsilon;

    vec3 gradient = vec3(
        GetClosetDistanceToCube(worldPosition + vec3(difference, 0.0f, 0.0f), cubeCenter, halfExtents) - GetClosetDistanceToCube( worldPosition - vec3(difference, 0.0f, 0.0f), cubeCenter, halfExtents),
        GetClosetDistanceToCube(worldPosition + vec3(0.0f, difference, 0.0f), cubeCenter, halfExtents) - GetClosetDistanceToCube( worldPosition - vec3(0.0f, difference, 0.0f), cubeCenter, halfExtents),
        GetClosetDistanceToCube(worldPosition + vec3(0.0f, 0.0f, difference), cubeCenter, halfExtents) - GetClosetDistanceToCube( worldPosition - vec3(0.0f, 0.0f, difference), cubeCenter, halfExtents));

    gradient = normalize(gradient);
    gradient *= -1.0f;

    return vec4(gradient, GetClosetDistanceToCube(worldPosition, cubeCenter, halfExtents));
}