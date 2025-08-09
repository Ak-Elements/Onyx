

vec4 GetSphereValueAndGradient(vec3 worldPosition, vec3 sphereCenter, float sphereRadius)
{
    const float Epsilon = 0.0001;

    vec3 gradient = worldPosition - sphereCenter;
    float distance = length(gradient + Epsilon);
    gradient = gradient * ( 1.0 / distance);

    return vec4(gradient, sphereRadius - distance);
}