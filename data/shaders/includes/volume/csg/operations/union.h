vec4 GetUnion(vec4 lhsGradient, vec4 rhsGradient)
{
    float chooseRhs = step(lhsGradient.w, rhsGradient.w);
    return mix(lhsGradient, rhsGradient, chooseRhs);
}