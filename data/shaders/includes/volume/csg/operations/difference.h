vec4 GetDifference(vec4 lhsGradient, vec4 rhsGradient)
{
    if (-lhsGradient.w > rhsGradient.w)
    {
        return -lhsGradient;
    }

    return rhsGradient;
}