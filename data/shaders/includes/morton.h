uint ToMortonCode(uint coordinate)
{
    uint morton = coordinate & 0x000003FF;
    morton = (morton | morton << 16) & 0x30000FF;
    morton = (morton | morton << 8) & 0x0300F00F;
    morton = (morton | morton << 4) & 0x30C30C3;
    morton = (morton | morton << 2) & 0x9249249;
    return morton;
}

uint FromMortonCode(uint morton)
{
    uint coord = morton & 0x9249249;
    coord = (coord ^ (coord >> 2)) & 0x30C30C3;
    coord = (coord ^ (coord >> 4)) & 0x0300F00F;
    coord = (coord ^ (coord >> 8)) & 0x30000FF;
    coord = (coord ^ (coord >> 16)) & 0x000003FF;
    return coord;
}

uint FromMortonCode(uint64_t morton)
{
    //0x1249249249249249
    //0x10C30C30C30C30C3
    //0x100F00F00F00F00F
    //0x1F0000FF0000FF
    //0x1F00000000FFFF
    //0x1FFFFF
    uint64_t coord = morton & uint64_t(1317624576693539401ul);
    coord = (coord ^ (coord >> 2)) & uint64_t(1207822528635744451ul);
    coord = (coord ^ (coord >> 4)) & uint64_t(1157144660301377551ul);
    coord = (coord ^ (coord >> 8)) & uint64_t(8725728556220671ul);
    coord = (coord ^ (coord >> 16)) & uint64_t(8725724278095871ul);
    coord = (coord ^ (coord >> 32)) & uint64_t(2097151ul);
    return uint(coord);
}

uint EncodeMorton(uvec3 coordinate)
{
    uint outMorton = ToMortonCode(coordinate.x) | (ToMortonCode(coordinate.y) << 1) | (ToMortonCode(coordinate.z) << 2);
    return outMorton;
}

uvec3 DecodeMorton(uint morton)
{
    uvec3 outCoordinate;
    outCoordinate.x = FromMortonCode(morton);
    outCoordinate.y = FromMortonCode(morton >> 1);
    outCoordinate.z = FromMortonCode(morton >> 2);
    return outCoordinate;
}

uvec3 DecodeMorton(uint64_t morton)
{
    uvec3 outCoordinate;
    outCoordinate.x = FromMortonCode(morton);
    outCoordinate.y = FromMortonCode(morton >> 1);
    outCoordinate.z = FromMortonCode(morton >> 2);
    return outCoordinate;
}