
struct CsgPlane
{
    vec3 Normal;
    float Distance;
};

vec4 GetValueAndGradient(vec3 worldPosition, CsgPlane plane)
{
    return vec4(
        plane.Normal.x,
        plane.Normal.y,
        plane.Normal.z,
        dot(plane.Normal, worldPosition) - plane.Distance);
}