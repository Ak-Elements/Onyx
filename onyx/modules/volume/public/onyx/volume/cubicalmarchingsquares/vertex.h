#pragma once

namespace Onyx::Volume
{
    namespace CubicalMarchingSquares
    {
        namespace Internal
        {
            template <typename Scalar>
            struct Vertex
            {
            private:
                using Vector3T = Vector3<Scalar>;

            public:
                Vector3T Position;
                Vector3T Normal;
            };
        }
    }
}