#pragma once

namespace onyx::volume {
enum class IsoSurfaceMethod : uint8_t { DMC, DmcWithCmsErrorMetric, CMS, None };

class VolumeBase;

template < typename Scalar >
class IsoSurface {
  public:
    static constexpr Scalar IsoLevel = 0;

    /// To call Marching Squares with a cube on its front.
    static constexpr uint8_t MsCornersFront[ 4 ] = { 7, 6, 2, 3 };

    /// To call Marching Squares with a cube on its back.
    static constexpr uint8_t MsCornersBack[ 4 ] = { 5, 4, 0, 1 };

    /// To call Marching Squares with a cube on its left.
    static constexpr uint8_t MsCornersLeft[ 4 ] = { 4, 7, 3, 0 };

    /// To call Marching Squares with a cube on its right.
    static constexpr uint8_t MsCornersRight[ 4 ] = { 6, 5, 1, 2 };

    /// To call Marching Squares with a cube on its top.
    static constexpr uint8_t MsCornersTop[ 4 ] = { 4, 5, 6, 7 };

    /// To call Marching Squares with a cube on its bottom.
    static constexpr uint8_t MsCornersBottom[ 4 ] = { 3, 2, 1, 0 };

    virtual ~IsoSurface() = default;

    virtual void addTriangles( const Vector3< Scalar > corners[],
                               const Vector4< Scalar > volumeValues[],
                               const Vector3< Scalar > valuesAnalyticalNormals[] ) const = 0;

  protected:
    explicit IsoSurface( const VolumeBase* volume )
        : m_volume( volume ) {}

    Vector3< Scalar > calculateSurfacePositionAndNormal( const Vector3< Scalar >& v0,
                                                         const Vector3< Scalar >& v1,
                                                         const Vector4< Scalar >& val0,
                                                         const Vector4< Scalar >& val1,
                                                         Vector3< Scalar >& outNormal ) const {
        using std::abs;
        using std::numeric_limits;
        const Scalar epsilon = numeric_limits< Scalar >::epsilon();
        if( abs( val0[ 3 ] - IsoLevel ) <= epsilon ) {
            outNormal[ 0 ] = val0[ 0 ];
            outNormal[ 1 ] = val0[ 1 ];
            outNormal[ 2 ] = val0[ 2 ];

            // outPosition = v0;
            return v0;
        }
        if( abs( val1[ 3 ] - IsoLevel ) <= epsilon ) {
            outNormal[ 0 ] = val1[ 0 ];
            outNormal[ 1 ] = val1[ 1 ];
            outNormal[ 2 ] = val1[ 2 ];
            // outPosition = v1;
            return v1;
        }
        if( abs( val1[ 3 ] - val0[ 3 ] ) <= epsilon ) {
            outNormal[ 0 ] = val0[ 0 ];
            outNormal[ 1 ] = val0[ 1 ];
            outNormal[ 2 ] = val0[ 2 ];
            // outPosition = v0;
            return v0;
        }

        // linear interpolation to get the point on the surface and the normal
        Scalar multiplier = ( IsoLevel - val0[ 3 ] ) / ( val1[ 3 ] - val0[ 3 ] );
        Vector4< Scalar > normal = val0 + multiplier * ( val1 - val0 );
        outNormal[ 0 ] = normal[ 0 ];
        outNormal[ 1 ] = normal[ 1 ];
        outNormal[ 2 ] = normal[ 2 ];
        outNormal.normalize();

        // outPosition[x] = v0 + multiplier * (v1 - v0);
        return v0 + multiplier * ( v1 - v0 );
    }

  protected:
    const VolumeBase* m_volume = nullptr;
};
} // namespace onyx::volume
