#pragma once

namespace Onyx::Volume
{

    namespace MarchingSquares_CMS
    {
        /*
        // Marching Squares
        //
        //
        //
        //      v0 +--e0--+ v2
        //         +      +
        //         e2     e3
        //         +      +
        //      v1 +--e1--+ v3
        //

        //   +------+
        //   +      + Configuration 0: 0
        //   +      +				~: 15
        //   +      +
        //   +------+
        //
        //    ------
        //   0      + Configuration 1:  1, 2, 4, 8
        //   +\     +		Mirrored~: 14,13,11, 7
        //   + \    +
        //   *--1---
        //
        //    ------
        //   +      + Configuration 2:  3, 5
        //   0------1		Mirrored~: 12,10
        //   +      +
        //   *------*
        //
        //    ---3--*
        //   0    \ + Configuration 3:  6
        //   +\    \+	    Mirrored~:  9       
        //   + \    2
        //   *--1---
        //
        //*/
        /*
        //
        //
        //      Vertex and Edge Index Map
        //
        //            2-------0------6
        //           /.             /|
        //          10.           11 |
        //         /  2           /  3
        //        /   .          /   |     ^ Y
        //       3-------5------7    |     |
        //       |    0 . . 1 . |. . 4     --> X
        //       |   .          |   /
        //       6  8           7  9
        //       | .            | /
        //       |.             |/
        //       1-------4------5       
        //
        //                 
        //
        //    
        */
        // For a given corners flag, this array defines
        // the edges from which we draw interior edges

        //      v2 +--e0--+ v0
        //         +      +
        //         e3     e2
        //         +      +
        //      v3 +--e1--+ v1

        static constexpr onyxS8 MS_TABLE[16][4] =
        {
            {-1,-1,-1,-1}, //  0: configuration 0
            { 0, 2,-1,-1}, //  1: configuration 1
            { 2, 1,-1,-1}, //  2: configuration 1
            { 0, 1,-1,-1}, //  3: configuration 2
            { 3, 0,-1,-1}, //  4: configuration 1
            { 3, 2,-1,-1}, //  5: configuration 2 
            { 3, 1, 2, 0}, // *6: configuration 3 - Ambiguous case {3,0,2,1}
            { 3, 1,-1,-1}, //  7: configuration 1
            { 1, 3,-1,-1}, //  8: configuration 1
            { 0, 3, 1, 2}, // *9: configuration 3 - Ambiguous case {1,3,0,2}
            { 2, 3,-1,-1}, // 10: configuration 2 
            { 0, 3,-1,-1}, // 11: configuration 1
            { 1, 0,-1,-1}, // 12: configuration 2
            { 1, 2,-1,-1}, // 13: configuration 1
            { 2, 0,-1,-1}, // 14: configuration 1
            {-1,-1,-1,-1}, // 15: configuration 0
        };

        static constexpr onyxS8 MS_TABLE_AMBIGUOUS[16][4] =
        {
            {-1,-1,-1,-1}, //  0: configuration 0
            {-1,-1,-1,-1}, //  1: configuration 1
            {-1,-1,-1,-1}, //  2: configuration 1
            {-1,-1,-1,-1}, //  3: configuration 2
            {-1,-1,-1,-1}, //  4: configuration 1
            {-1,-1,-1,-1}, //  5: configuration 2
            { 3, 0, 2, 1}, // *6: configuration 3 : Ambiguous case {1,0,3,2}
            {-1,-1,-1,-1}, //  7: configuration 1
            {-1,-1,-1,-1}, //  8: configuration 1
            { 1, 3, 0, 2}, // *9: configuration 3 : Ambiguous case {1,3,0,2}
            {-1,-1,-1,-1}, // 10: configuration 2
            {-1,-1,-1,-1}, // 11: configuration 1
            {-1,-1,-1,-1}, // 12: configuration 2
            {-1,-1,-1,-1}, // 13: configuration 1
            {-1,-1,-1,-1}, // 14: configuration 1
            {-1,-1,-1,-1}, // 15: configuration 0
        };


        // Indexed by edge number, returns vertex index
        static constexpr onyxU8 VERTEX_MAP[4][2] = {
            {0, 2},
            {3, 1},
            {1, 0},
            {2, 3}
        };

        static const onyxU8 FACE_AXES[6][2] = {
            {0 | 2, 8 | 4},
            {8 | 2, 8 | 4},
            {8 | 1, 8 | 4},
            {8 | 1, 0 | 4},
            {8 | 1, 0 | 2},
            {8 | 1, 8 | 2}
        };


        // returns the axis to use on this face e.g face 0 x/z
        static const onyxU8 FACE_COORDS[6][2] = {
            {0, 1}, // face 0
            {0, 1}, // face 1
            {0, 2}, // face 2
            {0, 2}, // face 3
            {1, 2}, // face 4
            {1, 2}  // face 5
        };

        static const onyxU8 FACE_EDGE_TO_CUBE_EDGE[6][4]
        {
            {0, 1, 3, 2},
            {4, 5, 7, 6},
            {1, 4, 9, 8},
            {0, 5,10,11},
            {2, 6, 8, 10},
            {3, 7, 11, 9},
        };

        // Not needed
        /* NEXT_FACE_EDGE[face][edge] gives the connected [face, edge] */
        static const onyxU8 NEXT_FACE_EDGE[6][4][2] = {
            {{3, 0}, {2, 0}, {4, 0}, {5, 0}}, // face 0
            {{2, 1}, {3, 1}, {4, 1}, {5, 1}}, // face 1
            {{0, 1}, {1, 0}, {4, 3}, {5, 2}}, // face 2
            {{0, 0}, {1, 1}, {5, 3}, {4, 2}}, // face 3
            {{0, 2}, {1, 2}, {3, 3}, {2, 2}}, // face 4
            {{0, 3}, {1, 3}, {2, 3}, {3, 2}}  // face 5
        };
    }
}