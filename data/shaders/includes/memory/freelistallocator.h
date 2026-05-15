#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_atomic_int64 : require

uint64_t packRange( uint start, uint size ) {
    return ( uint64_t( start ) << 32u ) | uint64_t( size );
}
uint unpackStart( uint64_t packed ) {
    return uint( packed >> 32u );
}
uint unpackSize( uint64_t packed ) {
    return uint( packed & 0xFFFFFFFFu );
}

layout( scalar, buffer_reference, buffer_reference_align = 4 ) buffer FreeListAllocator {
    uint Capacity;
    uint RangesCount;
    uint64_t Ranges[]; // high 32 = StartIndex, low 32 = Size
};

int findRange( FreeListAllocator allocator, uint count, out uint64_t packedOut ) {
    int bestIdx = -1;
    uint bestSize = 0xFFFFFFFFu;

    for( int i = 0; i < int( allocator.RangesCount ); ++i ) {
        uint64_t packed = allocator.Ranges[ i ];
        uint size = unpackSize( packed );
        if( count <= size && size < bestSize ) {
            bestSize = size;
            bestIdx = i;
            packedOut = packed;
            if( size == count )
                break; // perfect fit, can't do better
        }
    }
    return bestIdx;
}

// Allocates 'count' slots. Returns the starting slot index, or -1 on failure.
int allocate( FreeListAllocator allocator, uint count ) {
    uint64_t expected;
    int rangeIdx = findRange( allocator, count, expected );
    if( rangeIdx == -1 )
        return -1;

    while( true ) {
        uint oldStart = unpackStart( expected );
        uint oldSize = unpackSize( expected );

        // Carve 'count' off the front of the range.
        uint newStart = oldStart + count;
        uint newSize = oldSize - count;
        uint64_t desired = packRange( newStart, newSize );

        uint64_t result = atomicCompSwap( allocator.Ranges[ rangeIdx ], expected, desired );

        if( result == expected ) {
            // CAS succeeded: return the base of the allocated block.
            return int( oldStart );
        }

        // CAS failed — another thread modified this range.
        // Use the value that was actually there (result) for the next attempt.
        expected = result;

        if( count <= unpackSize( expected ) ) {
            // Still fits in the same range, retry with the fresh snapshot.
            continue;
        }

        // No longer fits — find a different range.
        rangeIdx = findRange( allocator, count, expected );
        if( rangeIdx == -1 )
            return -1;
    }
}

struct FreedBlock {
    uint start;
    uint size;
};

layout( buffer_reference ) buffer FreedBlocks {
    uint count; // atomicAdd here during unload
    FreedBlock blocks[];
};

void free( FreeListAllocator allocator, int start, int size ) {
    // append / increase size of a range that is next to the free'd area
    // or create a new range if no existing range is found that matches
    // we should add a seperate pass that merges ranges over time if they get out of sync
    // but that can run every N'th frame
}
