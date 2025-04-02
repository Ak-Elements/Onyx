#pragma once

namespace Onyx::Volume
{
    class VolumeBase
    {
    public:
        // TODO: Check if needed
        static const unsigned int VOLUME_CHUNK_ID;
        static const unsigned int VOLUME_CHUNK_VERSION;
        static const size_t SERIALIZATION_CHUNK_SIZE;

        virtual ~VolumeBase() {}

        // TODO - Check if we can avoid creating Vector4f's 
        virtual Vector4f GetValueAndGradient(const Vector3f& position) const = 0;
        virtual onyxF32 GetValue(const Vector3f& position) const = 0;

        // most likely needed in some kind of form - lets see
        //void Serialize(const Vector3f& from, const Vector3f to, onyxF32 voxelWidth, onyxF32 maxClampedAbsoluteDensity, const char* file);

        // dont know what this is needed for yet
        //bool GetFirstRayIntersection(const /*todo ray*/ int& ray, Vector3f& result, onyxF32 scale, size_t maxIterations, onyxF32 maxDistance) const;

        onyxF32 GetVolumeSpaceToWorldSpaceFactor() const { return 1.0f; };
    protected:
        // dont know what this is needed for yet
        //virtual Vector3f GetIntersectionStart(const /*todo ray*/ int& ray, onyxF32 maxDistance) const;
        // dont know what this is needed for yet
       // virtual Vector3f GetIntersectionEnd(const /*todo ray*/ int& ray, onyxF32 maxDistance) const;
    };

}
