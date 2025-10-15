
vec4 GetUnion(vec4 lhsGradient, vec4 rhsGradient)
{
    if (lhsGradient.w < rhsGradient.w)
    {
        return lhsGradient;
    }

    return rhsGradient;
}

vec4 GetUnionSmooth(vec4 lhsGradient, vec4 rhsGradient, float smoothness)
{
    //float h = clamp(0.5 + 0.5 * (rhsGradient.w - lhsGradient.w) / smoothness, 0.0, 1.0);
    //float d = mix(rhsGradient.w, lhsGradient.w, h) - smoothness * h * (1.0 - h);

    smoothness *= 4.0;
    float h = max(smoothness - abs(lhsGradient.w - rhsGradient.w), 0.0)/(2.0 * smoothness);
    return vec4(mix(lhsGradient.xyz,rhsGradient.xyz,( lhsGradient.w < rhsGradient.w ) ? h : 1.0-h ), min( lhsGradient.w,rhsGradient.w ) - h * h * smoothness);

    //smoothness *= 4.0;
    //float h = max(smoothness-abs(lhsGradient.w - rhsGradient.w),0.0);
    //float d = min(lhsGradient.w, rhsGradient.w) - h*h*0.25/smoothness;
    //vec3 g = normalize(mix(rhsGradient.xyz, lhsGradient.xyz, h));  // blend gradients
//
    //return vec4(g, d);
}