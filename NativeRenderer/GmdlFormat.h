#pragma once

#include <cstdint>

static constexpr char GMDL_MAGIC[4] = { 'G', 'M', 'D', 'L' };
static constexpr uint32_t GMDL_VERSION = 1;

// Flags for optional vertex data
enum GmdlFlags : uint32_t
{
    GMDL_HAS_NORMALS = 1 << 0,
    GMDL_HAS_UVS = 1 << 1,
    GMDL_HAS_TANGENTS = 1 << 2,
    // Future bits reserved
};

#pragma pack(push, 1) // No padding!

struct GmdlHeader
{
    char magic[4];          // 'GMDL'
    uint32_t version;       // File version
    uint32_t vertexCount;   // Number of vertices
    uint32_t indexCount;    // Number of indices
    uint32_t vertexStride;  // Size of one vertex in bytes
    uint32_t flags;         // Bitmask: normals, uvs, tangents, etc.
    uint32_t materialNameLength; // Length in bytes (not null-terminated)
    uint32_t reserved;      // Reserved for future use
};

#pragma pack(pop)



