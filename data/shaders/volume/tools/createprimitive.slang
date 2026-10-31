import includes.volume.volumesource;

import includes.volume.csg.cube;
import includes.volume.csg.ellipsoid;
import includes.volume.csg.sphere;
import includes.volume.csg.plane;

import includes.volume.csg.operations.difference;
import includes.volume.csg.operations.intersect;
import includes.volume.csg.operations.union;

struct RayHit {
    float3 Position;
    uint HasHit;
};

struct PushConstants {
    Ptr< VolumeSourceList > VolumeSourcesList;
    Ptr< VolumeSourcesData > VolumeSourcesData;

    Ptr< RayHit > HitResult;
    uint32_t PrimitiveType; // Push constants do not support scalar layout so we have to use uint32_t
    uint32_t Operation;

    float3 BrushSize;
};

[ push_constant ]
PushConstants Constants;

void createVolumeSource( uint sourceIndex, float3 hitPosition, float3 brushSize ) {
    // // TODO: offset based on memory size
    // // TODO: Add other brush types
    uint dataStartIndex = sourceIndex * VolumePackedDataSize;
    switch( VolumeSourceType( Constants.PrimitiveType ) ) {
    case VolumeSourceType::Sphere: {
        CsgSphere sphere = CsgSphere( hitPosition, brushSize.x );
        sphere.pack( Constants.VolumeSourcesData, dataStartIndex );
        break;
    }
    case VolumeSourceType::Cube: {
        CsgCube cube = CsgCube( hitPosition, brushSize );
        cube.pack( Constants.VolumeSourcesData, dataStartIndex );
        break;
    }
    case VolumeSourceType::Ellipsoid: {
        CsgEllipsoid ellipsoid = CsgEllipsoid( hitPosition, brushSize );
        ellipsoid.pack( Constants.VolumeSourcesData, dataStartIndex );
        break;
    }
    }
}

[ shader( "compute" ) ]
void computeMain() {
    if( Constants.HitResult.HasHit == 0 )
        return;

    float3 hitPosition = Constants.HitResult.Position;

    uint volumeSourceIndex = 0;
    InterlockedAdd( Constants.VolumeSourcesList.Count, 1, volumeSourceIndex );
    Constants.VolumeSourcesList.Sources[ volumeSourceIndex ] = VolumeSource(
        VolumeSourceType( Constants.PrimitiveType ),
        VolumeOperation( Constants.Operation ),
        volumeSourceIndex );

    createVolumeSource( volumeSourceIndex, hitPosition, Constants.BrushSize );
}
