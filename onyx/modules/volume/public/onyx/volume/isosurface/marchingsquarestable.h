#pragma once
#pragma once

namespace Onyx::Volume
{

    namespace MarchingSquares
    {
        /** Marching Cubes tables from the public domain code found on
        http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
        in
        http://paulbourke.net/geometry/polygonise/marchingsource.cpp
        */

        /*
        Throw in the corners inside of the volume and get a 4 bit array which edges are cutting it.
        Edge indices to check then (Edge between 0 and 4 cut == msEdges[x] & 4).
        0-1-1
        |   |
        4   2
        |   |
        3-3-2
        */
        const int Edges[16] =
        {
            0x0, // 0000 Nothing intersects
            0x9, // 0001 0
            0x3, // 0010 1
            0xA, // 0011 0 1
            0x6, // 0100 2
            0xF, // 0101 0 2
            0x5, // 0110 1 2
            0xC, // 0111 0 1 2
            0xC, // 1000 3
            0x5, // 1001 0 3
            0xF, // 1010 1 3
            0x6, // 1011 0 1 3
            0xA, // 1100 2 3
            0x3, // 1101 0 2 3
            0x9, // 1110 1 2 3
            0x0  // 1111 0 1 2 3
        };

        /*
        To get a triangulation with up to 5 triangles based on the corners inside. Alternative
        ambigous cases added at the end. Indices of the counterclockwise formed triangles:
        0-1-2
        |   |
        7   3
        |   |
        6-5-4
        */
        const int Triangles[18][13] =
        {
            { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Nothing intersects
            { 7, 1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },    // 0
            { 3, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },    // 1
            { 7, 2, 0, 7, 3, 2, -1, -1, -1, -1, -1, -1, -1 },       // 0 1
            { 5, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },    // 2
            { 7, 1, 0, 3, 5, 4, -1, -1, -1, -1, -1, -1, -1 },       // 0 2
            { 1, 4, 2, 1, 5, 4, -1, -1, -1, -1, -1, -1, -1 },       // 1 2
            { 0, 7, 5, 0, 5, 4, 0, 4, 2, -1, -1, -1, -1 },          // 0 1 2
            { 7, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },    // 3
            { 0, 5, 1, 0, 6, 5, -1, -1, -1, -1, -1, -1, -1 },       // 0 3
            { 1, 3, 2, 7, 6, 5, -1, -1, -1, -1, -1, -1, -1 },       // 1 3
            { 3, 6, 5, 3, 0, 6, 3, 2, 0, -1, -1, -1, -1 },          // 0 1 3
            { 7, 6, 4, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1 },       // 2 3
            { 3, 1, 0, 3, 0, 6, 3, 6, 4, -1, -1, -1, -1 },          // 0 2 3
            { 2, 1, 7, 2, 7, 6, 2, 6, 4, -1, -1, -1, -1 },          // 1 2 3
            { 6, 2, 0, 6, 4, 2, -1, -1, -1, -1, -1, -1, -1 },       // 0 1 2 3
            { 1, 0, 7, 1, 7, 5, 1, 5, 4, 1, 4, 3, -1 },             // 0 2 alternative
            { 1, 3, 2, 1, 5, 3, 1, 6, 5, 1, 7, 6, -1 }              // 1 3 alternative
        };
    }
}