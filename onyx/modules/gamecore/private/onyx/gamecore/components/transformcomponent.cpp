#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>

namespace onyx::game_core::world_transform {
void setRotation( TransformComponent& transformComponent,
                  const Rotor3f32& rotation,
                  const EulerRadiansF32& eulerAngles ) {
    if( transformComponent.Rotation == rotation ) {
        return;
    }

    transformComponent.Rotation = rotation;
    transformComponent.RotationEuler = eulerAngles;
}

void setRotation( TransformComponent& transformComponent, const Rotor3f32& rotation ) {
    if( transformComponent.Rotation == rotation ) {
        return;
    }

    transformComponent.Rotation = rotation;
    transformComponent.RotationEuler = transformComponent.Rotation.toEulerAngles();
}

void setRotation( TransformComponent& transformComponent, const EulerRadiansF32& eulerAngles ) {
    if( transformComponent.RotationEuler == eulerAngles )
        return;

    transformComponent.RotationEuler = eulerAngles;
    transformComponent.Rotation = Rotor3f32( eulerAngles );
}

void rotate( TransformComponent& transformComponent, const EulerRadiansF32& eulerAngles ) {
    const EulerRadiansF32& newEuler = transformComponent.RotationEuler + eulerAngles;
    setRotation( transformComponent, newEuler );
}

Matrix3< float32 > getScaleMatrix( const TransformComponent& transformComponent ) {
    Vector3< float32 > column0{ 1, 0, 0 };
    Vector3< float32 > column1{ 0, 1, 0 };
    Vector3< float32 > column2{ 0, 0, 1 };

    return Matrix3{ column0 * transformComponent.Scale.X,
                    column1 * transformComponent.Scale.Y,
                    column2 * transformComponent.Scale.Z };
}

Matrix4< float32 > getTransform( const TransformComponent& transformComponent ) {
    // Translation matrix from Translation
    // * Rotation (quaternion / rotor3d)
    // * Scale matrix

    Matrix4< float32 > transformMatrix;
    transformMatrix[ 3 ] = transformMatrix[ 0 ] * transformComponent.Translation[ 0 ] +
                           transformMatrix[ 1 ] * transformComponent.Translation[ 1 ] +
                           transformMatrix[ 2 ] * transformComponent.Translation[ 2 ] + transformMatrix[ 3 ];

    Matrix3< float32 > scaleMatrix = getScaleMatrix( transformComponent );
    Matrix4< float32 > scaleMatrix4d{ Vector4( scaleMatrix[ 0 ], 0.0f ),
                                      Vector4( scaleMatrix[ 1 ], 0.0f ),
                                      Vector4( scaleMatrix[ 2 ], 0.0f ),
                                      Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) };

    return transformMatrix * transformComponent.Rotation.toMatrix4() * scaleMatrix4d;
}
} // namespace onyx::game_core::world_transform
