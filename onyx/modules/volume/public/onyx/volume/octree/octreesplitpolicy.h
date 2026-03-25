#pragma once

#include <onyx/volume/octree/octreenode.h>

namespace onyx::volume {
class VolumeBase;
struct VolumeDataContainer;

// remove this template argument
template < typename Scalar >
class OctreeSplitPolicy {
  public:
    OctreeSplitPolicy( uint8_t maxOctreeLevel, Scalar octreeRootSize, const VolumeBase& volumeBase )
        : m_OctreeRootSize( octreeRootSize )
        , m_MaxOctreeLevel( maxOctreeLevel )
        , m_VolumeSource( &volumeBase ) {}
    virtual ~OctreeSplitPolicy() = default;

    virtual bool ShouldSplit( OctreeNode< UniquePtr< VolumeDataContainer > >& node,
                              const Vector3< Scalar >& nodeWorldPosition,
                              Scalar halfExtent,
                              uint8_t nodeLevel ) = 0;

  protected:
    const Scalar m_OctreeRootSize;
    const uint8_t m_MaxOctreeLevel;
    const VolumeBase* m_VolumeSource;
};

} // namespace onyx::volume
