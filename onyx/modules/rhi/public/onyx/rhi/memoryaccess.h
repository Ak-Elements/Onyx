#pragma once

namespace onyx::rhi {
enum class GPUAccess : uint8_t {
    Read,
    Write,
    Staging, // copying in/out of GPU only
};

enum class CPUAccess : uint8_t {
    None,
    UpdateUnsynchronized,
    UpdateKeep,
    Read, // mostly for staging
    Write,
    ReadWrite
};

enum class MapMode : uint8_t {
    Write = 1 << 0,
    Read = 1 << 1,
    ReadWrite = Write | Read,

    // unsynchronized requires external synchronization
    Unsynchronized = 1 << 2,
    WriteUnsynchronized = Write | Unsynchronized,
    ReadUnsynchronized = Read | Unsynchronized,
    ReadWriteUnsynchronized = ReadWrite | Unsynchronized,
};
} // namespace onyx::rhi