#pragma once

#include <array>
#include <vector>

// Each entry holds {vertex, texcoord, normal} indices (0-based, -1 if absent).
struct Face {
    std::vector<std::array<int, 3>> vertexIndices;
};
