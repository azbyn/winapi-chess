#pragma once

#include "Utils.h"

#include <iostream>
#include <array>

namespace core {

/*
  by making it a template the conversion from string array to sprite
  is done at compile time. We don't have constexpr memory allocation
  yet (C++20), so we can't use that.
 */
template<size_t Width, size_t Height, const char CharPalette_[]>
class PaletteSprite {
public:
    // a string view is nicer to work with than a c string
    static constexpr std::string_view CharPalette = CharPalette_;
    static constexpr size_t ColorCount = CharPalette.length();

    constexpr size_t width() const { return Width; }
    constexpr size_t height() const { return Height; }

    constexpr Point size() const { return {Width, Height}; }
private:
    //indices to a palette of colors
    // 32 colors max
    std::array<uint8_t, Width * Height> data = {};
    //uint8_t data[Width*Height];

    constexpr uint8_t indexFromChar(char c) {
        size_t res = CharPalette.find(c);
        constexpr_assert(res < ColorCount,
                         "Invalid char. only chars in CharPalette are allowed");
        return (uint8_t)res;
    }

public:
    //takes Height number of strings of size Width each.
    constexpr PaletteSprite(std::initializer_list<std::string_view> strings);
    //while not technically wrong we should never copy a PaletteSprite
    PaletteSprite(const PaletteSprite&) = delete;
    PaletteSprite(PaletteSprite&&) = delete;
    PaletteSprite& operator=(const PaletteSprite&) = delete;
    PaletteSprite& operator=(PaletteSprite&&) = delete;

    constexpr uint8_t& operator()(size_t x, size_t y) {
        return data[y * Width + x];
    }
    constexpr uint8_t operator()(size_t x, size_t y) const {
        return data[y * Width + x];
    }
    friend std::ostream& operator<<(std::ostream& out, const PaletteSprite& s) {
        out <<"{";

        for (size_t j = 0; j < Height; ++j) {
            for (size_t i = 0; i < Width; ++i) {
                out << (int)s(i, j)<< ",";
            }
            out << "\n";
        }
        return out <<"}";
    }
};



template<size_t Width, size_t Height, const char CharPalette_[]>
constexpr PaletteSprite<Width, Height, CharPalette_>::PaletteSprite(
        std::initializer_list<std::string_view> strings) {
    constexpr_assert(strings.size() == Height,
                     "The number of strings provided must be equal to Height");
    size_t j = 0;
    for (auto str: strings) {
        constexpr_assert(str.size() == Width,
                         "Each string size must be Width");
        size_t i = 0;
        for (auto c: str) {
            (*this)(i++, j) = indexFromChar(c);
        }
        ++j;
    }
}

} // namespace core
