
const float Epsilon = 0.0001;

vec4 GetSphereValueAndGradient(vec3 worldPosition, vec3 sphereCenter, float sphereRadius)
{
    vec3 gradient = worldPosition - sphereCenter;
    float length = length(gradient);
    gradient = gradient * ( 1.0 / (length + Epsilon));

    return vec4(gradient, sphereRadius - length);
}

vec4 GetPlaneValueAndGradient(vec3 worldPosition, vec3 normal, float distance)
{
    return vec4(
        normal[0],
        normal[1],
        normal[2],
        distance - normal.dot(worldPosition));
}