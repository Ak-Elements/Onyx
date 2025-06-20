
const float Epsilon = 0.0001;

vec4 GetSphereValueAndGradient(vec3 worldPosition, vec3 sphereCenter, float sphereRadius)
{
    vec3 gradient = worldPosition - sphereCenter;
    float distance = length(gradient + Epsilon);
    gradient = gradient * ( 1.0 / distance);

    return vec4(gradient, sphereRadius - distance);
}

vec4 GetPlaneValueAndGradient(vec3 worldPosition, vec3 normal, float distance)
{
    return vec4(
        normal.x,
        normal.y,
        normal.z,
        distance - dot(normal, worldPosition));
}
