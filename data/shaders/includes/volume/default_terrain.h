vec4 SampleBaseTerrainValue(vec3 worldPosition)
{
    CsgPlane plane = CsgPlane(vec3(0,1,0), 0);
    CsgSphere sphere = CsgSphere(vec3(0,0,-1000), 500);
    return GetUnion(GetValueAndGradient(worldPosition, sphere), GetValueAndGradient(worldPosition, plane));
}