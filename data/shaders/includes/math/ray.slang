
// From https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code.
float rayIntersectSphere( float3 ro, float3 rd, float radius ) {
    float b = dot( ro, rd );
    float c = dot( ro, ro ) - radius * radius;
    if( c > 0.0f && b > 0.0 )
        return -1.0;
    float discr = b * b - c;
    if( discr < 0.0 )
        return -1.0;
    // Special case: inside sphere, use far discriminant
    if( discr > b * b )
        return ( -b + sqrt( discr ) );
    return -b - sqrt( discr );
}
