
struct CsgEllipsoid
{
    vec3 Position;
    vec3 Radii;
};

void PackCsgSource(WorldVolumeSources volumeSourcesData, uint sourceIndex, CsgEllipsoid ellipsoid)
{
    volumeSourcesData.SourcesData[sourceIndex + 0] = ellipsoid.Position.x;
    volumeSourcesData.SourcesData[sourceIndex + 1] = ellipsoid.Position.y;
    volumeSourcesData.SourcesData[sourceIndex + 2] = ellipsoid.Position.z;
    volumeSourcesData.SourcesData[sourceIndex + 3] = ellipsoid.Radii.x;
    volumeSourcesData.SourcesData[sourceIndex + 4] = ellipsoid.Radii.y;
    volumeSourcesData.SourcesData[sourceIndex + 5] = ellipsoid.Radii.z;
}

void UnpackCsgSource(WorldVolumeSources volumeSourcesData, uint sourceIndex, out CsgEllipsoid outEllipsoid)
{
    outEllipsoid.Position.x = volumeSourcesData.SourcesData[sourceIndex];
    outEllipsoid.Position.y = volumeSourcesData.SourcesData[sourceIndex + 1];
    outEllipsoid.Position.z = volumeSourcesData.SourcesData[sourceIndex + 2];
    outEllipsoid.Radii.x = volumeSourcesData.SourcesData[sourceIndex + 3];
    outEllipsoid.Radii.y = volumeSourcesData.SourcesData[sourceIndex + 4];
    outEllipsoid.Radii.z = volumeSourcesData.SourcesData[sourceIndex + 5];
}

vec4 GetValueAndGradient(vec3 worldPosition, CsgEllipsoid ellipsoid)
{
    vec3 difference = worldPosition - ellipsoid.Position;
    vec3 radiiSquared = ellipsoid.Radii * ellipsoid.Radii;
    float k0 = length(difference / ellipsoid.Radii);

    vec3 gradient = difference / radiiSquared;
    float k1 = length(gradient);
    return vec4((gradient / k1), k0 * (k0 - 1.0) / k1);
}