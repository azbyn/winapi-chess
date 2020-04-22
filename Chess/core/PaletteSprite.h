#pragma once

#include "Color.h"

#include <iostream>

namespace core {
class PaletteSprite {
public:
    virtual size_t width() const = 0;
    virtual size_t height() const = 0;

    Point size() const { return {(int)width(), (int)height()}; }

    virtual uint8_t at(size_t x, size_t y) const = 0;
    friend std::ostream& operator<<(std::ostream& out, const PaletteSprite& s) {
        out <<"{";

        for (size_t j = 0; j < s.height(); ++j) {
            for (size_t i = 0; i < s.width(); ++i) {
                out << (int)s.at(i, j)<< ",";
            }
            out << "\n";
        }
        return out <<"}";
    }
};
// First color is reserved for transparency
class Palette {
public:
    constexpr Palette() = default;
    virtual Color operator[](uint8_t) const = 0;
};

} // namespace core
