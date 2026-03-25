#pragma once

namespace onyx::volume {
class VolumeBase {
  public:
    // TODO: Check if needed
    static const unsigned int VOLUME_CHUNK_ID;
    static const unsigned int VOLUME_CHUNK_VERSION;
    static const size_t SERIALIZATION_CHUNK_SIZE;

    virtual ~VolumeBase() {}

    // TODO - Check if we can avoid creating Vector4f's
    virtual Vector4f32 GetValueAndGradient( const Vector3f32& position ) const = 0;
    virtual float32 GetValue( const Vector3f32& position ) const = 0;

    // most likely needed in some kind of form - lets see
    // void Serialize(const Vector3f& from, const Vector3f to, float32 voxelWidth, float32 maxClampedAbsoluteDensity,
    // const char* file);

    // dont know what this is needed for yet
    // bool GetFirstRayIntersection(const /*todo ray*/ int& ray, Vector3f& result, float32 scale, size_t maxIterations,
    // float32 maxDistance) const;

    float32 GetVolumeSpaceToWorldSpaceFactor() const { return 1.0f; };

  protected:
    // dont know what this is needed for yet
    // virtual Vector3f GetIntersectionStart(const /*todo ray*/ int& ray, float32 maxDistance) const;
    // dont know what this is needed for yet
    // virtual Vector3f GetIntersectionEnd(const /*todo ray*/ int& ray, float32 maxDistance) const;
};

} // namespace onyx::volume
