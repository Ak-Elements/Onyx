
vec4 GetPlaneValueAndGradient(vec3 worldPosition, vec3 normal, float distance)
{
    return vec4(
        normal.x,
        normal.y,
        normal.z,
        distance - dot(normal, worldPosition));
}