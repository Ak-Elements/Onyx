
struct CsgCube
{
    vec3 Position;
    vec3 HalfExtents;
};

void PackCsgCube(WorldVolumeSources volumeSourcesData, CsgCube cube, uint sourceIndex)
{
    volumeSourcesData.SourcesData[sourceIndex] = cube.Position.x;
    volumeSourcesData.SourcesData[sourceIndex + 1] = cube.Position.y;
    volumeSourcesData.SourcesData[sourceIndex + 2] = cube.Position.z;
    volumeSourcesData.SourcesData[sourceIndex + 3] = cube.HalfExtents.x;
    volumeSourcesData.SourcesData[sourceIndex + 4] = cube.HalfExtents.y;
    volumeSourcesData.SourcesData[sourceIndex + 5] = cube.HalfExtents.z;
}

CsgCube UnpackCsgCube(WorldVolumeSources volumeSourcesData, uint sourceIndex)
{
    CsgCube cube;
    cube.Position.x = volumeSourcesData.SourcesData[sourceIndex];
    cube.Position.y = volumeSourcesData.SourcesData[sourceIndex + 1];
    cube.Position.z = volumeSourcesData.SourcesData[sourceIndex + 2];
    cube.HalfExtents.x = volumeSourcesData.SourcesData[sourceIndex + 3];
    cube.HalfExtents.y = volumeSourcesData.SourcesData[sourceIndex + 4];
    cube.HalfExtents.z = volumeSourcesData.SourcesData[sourceIndex + 5];
    return cube;
}

float GetClosetDistanceToCube(vec3 worldPosition, vec3 cubeCenter, vec3 halfExtents)
{
    float x = max(worldPosition.x - cubeCenter.x - halfExtents.x,
        cubeCenter.x - worldPosition.x - halfExtents.x);

    float y = max(worldPosition.y - cubeCenter.y - halfExtents.y,
        cubeCenter.y - worldPosition.y - halfExtents.y);

    float z = max(worldPosition.z - cubeCenter.z - halfExtents.z,
        cubeCenter.z - worldPosition.z - halfExtents.z);
    
    float d = max(x, max(y, z));
    return d;
}

vec4 GetValueAndGradient(vec3 worldPosition, CsgCube cube)
{
    const float Epsilon = 0.0001;
    const float difference = Epsilon;

    vec3 gradient = vec3(
        GetClosetDistanceToCube(worldPosition + vec3(difference, 0.0f, 0.0f), cube.Position, cube.HalfExtents) - GetClosetDistanceToCube( worldPosition - vec3(difference, 0.0f, 0.0f), cube.Position, cube.HalfExtents),
        GetClosetDistanceToCube(worldPosition + vec3(0.0f, difference, 0.0f), cube.Position, cube.HalfExtents) - GetClosetDistanceToCube( worldPosition - vec3(0.0f, difference, 0.0f), cube.Position, cube.HalfExtents),
        GetClosetDistanceToCube(worldPosition + vec3(0.0f, 0.0f, difference), cube.Position, cube.HalfExtents) - GetClosetDistanceToCube( worldPosition - vec3(0.0f, 0.0f, difference), cube.Position, cube.HalfExtents));

    return vec4(gradient, GetClosetDistanceToCube(worldPosition, cube.Position, cube.HalfExtents));
}