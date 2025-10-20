vec4 SampleBaseTerrainValue(vec3 worldPosition)
{
    // move to sample function
    vec3 planeNormal = vec3(0,1,0);
    CsgPlane plane = CsgPlane(vec3(0,1,0), 10);
    CsgSphere sphere = CsgSphere(vec3(0,0,-1000), 500);

    vec4 planeGradient = GetValueAndGradient(worldPosition, plane);
    vec4 sphereGradient = GetValueAndGradient(worldPosition, sphere);
    return GetUnion(planeGradient, sphereGradient);
}