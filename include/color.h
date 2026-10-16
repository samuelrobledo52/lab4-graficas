#pragma once

#include <algorithm>
#include <cstdint>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color() : r(0), g(0), b(0) {}

    Color(int red, int green, int blue)
        : r(static_cast<uint8_t>(std::clamp(red, 0, 255))),
          g(static_cast<uint8_t>(std::clamp(green, 0, 255))),
          b(static_cast<uint8_t>(std::clamp(blue, 0, 255))) {}
};
