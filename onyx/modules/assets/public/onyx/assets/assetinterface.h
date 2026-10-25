#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

#include <onyx/assets/assetformat.h>
#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>

namespace onyx::assets {

enum class AssetState : uint8_t {
    Invalid = 0, // asset is in undefined invalid state
    Loading,
    Loaded,
    Missing, // asset is defined but can't be found
};

class AssetInterface : public RefCounted {
    friend struct AssetLoadRequest;
#if ONYX_IS_EDITOR
    friend struct AssetSaveRequest;
#endif
  public:
    void setState( AssetState state ) { m_state = state; }

    [[nodiscard]] bool isLoading() const { return m_state == AssetState::Loading; }
    [[nodiscard]] bool isLoaded() const { return m_state == AssetState::Loaded; }

  private:
    virtual void onLoadFinished( AssetId id, AssetState state ) = 0;
#if ONYX_IS_EDITOR
    virtual void onSaveFinished( AssetId id, bool success ) const = 0;
#endif

  protected:
    Atomic< AssetState > m_state = AssetState::Invalid;
};

} // namespace onyx::assets
