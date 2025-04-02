#pragma once

namespace Onyx::Volume
{
    enum class IsoSurfaceMethod
    {
        DMC,
        DMC_WITH_CMS_ERROR_METRIC,
        CMS,
        None
    };

    class VolumeBase;

    template <typename Scalar>
    class IsoSurface
    {
    public:
        static constexpr Scalar ISO_LEVEL = 0;

        /// To call Marching Squares with a cube on its front.
        static constexpr onyxU8 MS_CORNERS_FRONT[4] = { 7, 6, 2, 3 };

        /// To call Marching Squares with a cube on its back.
        static constexpr onyxU8 MS_CORNERS_BACK[4] = { 5, 4, 0, 1 };

        /// To call Marching Squares with a cube on its left.
        static constexpr onyxU8 MS_CORNERS_LEFT[4] = { 4, 7, 3, 0 };

        /// To call Marching Squares with a cube on its right.
        static constexpr onyxU8 MS_CORNERS_RIGHT[4] = { 6, 5, 1, 2 };

        /// To call Marching Squares with a cube on its top.
        static constexpr onyxU8 MS_CORNERS_TOP[4] = { 4, 5, 6, 7 };

        /// To call Marching Squares with a cube on its bottom.
        static constexpr onyxU8 MS_CORNERS_BOTTOM[4] = { 3, 2, 1, 0 };

        virtual ~IsoSurface() {}

        virtual void AddTriangles(const Vector3<Scalar> corners[], const Vector4<Scalar> volumeValues[], const Vector3<Scalar> valuesAnalyticalNormals[]) const = 0;

    protected:
        explicit IsoSurface(const VolumeBase* volume)
            : m_Volume(volume)
        {    
        }

        Vector3<Scalar> CalculateSurfacePositionAndNormal(const Vector3<Scalar>& v0, const Vector3<Scalar>& v1, const Vector4<Scalar>& val0, const Vector4<Scalar>& val1, Vector3<Scalar>& outNormal) const
        {
            using std::abs;
            using std::numeric_limits;
            const Scalar epsilon = numeric_limits<Scalar>::epsilon();
            if (abs(val0[3] - ISO_LEVEL) <= epsilon)
            {
                outNormal[0] = val0[0];
                outNormal[1] = val0[1];
                outNormal[2] = val0[2];

                //outPosition = v0;
                return v0;
            }
            if (abs(val1[3] - ISO_LEVEL) <= epsilon)
            {
                outNormal[0] = val1[0];
                outNormal[1] = val1[1];
                outNormal[2] = val1[2];
                //outPosition = v1;
                return v1;
            }
            if (abs(val1[3] - val0[3]) <= epsilon)
            {
                outNormal[0] = val0[0];
                outNormal[1] = val0[1];
                outNormal[2] = val0[2];
                //outPosition = v0;
                return v0;
            }

            // linear interpolation to get the point on the surface and the normal
            Scalar multiplier = (ISO_LEVEL - val0[3]) / (val1[3] - val0[3]);
            Vector4<Scalar> normal = val0 + multiplier * (val1 - val0);
            outNormal[0] = normal[0];
            outNormal[1] = normal[1];
            outNormal[2] = normal[2];
            outNormal.Normalize();

            //outPosition[x] = v0 + multiplier * (v1 - v0);
            return v0 + multiplier * (v1 - v0);
        }

    protected:
        const VolumeBase* m_Volume = nullptr;
    };
}

