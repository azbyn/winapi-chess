#pragma once

#include "PaletteSprite.h"

#include <array>

namespace core {
/*
  By making it a template, the conversion from string array to sprite
  is done at compile time.
 */
template<size_t Width, size_t Height, const char CharPalette_[]>
class ConstPaletteSprite : public PaletteSprite {
public:
    // a string view is nicer to work with than a c string
    static constexpr std::string_view CharPalette = CharPalette_;
    static constexpr size_t ColorCount = CharPalette.length();

    size_t width() const override { return Width; }
    size_t height() const override { return Height; }

    // Takes Height number of strings of size Width each.
    constexpr ConstPaletteSprite(std::initializer_list<std::string_view> strs);
    // While not technically wrong we should never copy a ConstPaletteSprite
    ConstPaletteSprite(const ConstPaletteSprite&) = delete;
    ConstPaletteSprite(ConstPaletteSprite&&) = delete;
    ConstPaletteSprite& operator=(const ConstPaletteSprite&) = delete;
    ConstPaletteSprite& operator=(ConstPaletteSprite&&) = delete;

    uint8_t at(size_t x, size_t y) const override {
        return data[y * Width + x];
    }

private:
    // Indices to a Palette
    std::array<uint8_t, Width * Height> data = {};
    //uint8_t data[Width*Height];
    constexpr uint8_t& at(size_t x, size_t y) {
        return data[y * Width + x];
    }


    constexpr uint8_t indexFromChar(char c) {
        size_t res = CharPalette.find(c);
        constexpr_assert(res < ColorCount,
                         "Invalid char. only chars in CharPalette are allowed");
        return (uint8_t)res;
    }
};

template<const char CharPalette_[]>
class ConstPalette : public Palette {
    // a string view is nicer to work with than a c string
    static constexpr std::string_view CharPalette = CharPalette_;
    static constexpr size_t ColorCount = CharPalette.length();

public:
    constexpr ConstPalette(std::initializer_list<Color> lst) : val() {
        int i = 0;
        for (auto& v : lst)
            val[i++] = v;
    }
    ConstPalette(const ConstPalette&) = delete;
    ConstPalette(ConstPalette&&) = delete;
    ConstPalette& operator=(const ConstPalette&) = delete;
    ConstPalette& operator=(ConstPalette&&) = delete;

    Color operator[](uint8_t i) const override {
        return val[i];
    }

private:
    std::array<Color, ColorCount> val;
};

template<size_t Width, size_t Height, const char CharPalette_[]>
constexpr ConstPaletteSprite<Width, Height, CharPalette_>::ConstPaletteSprite(
        std::initializer_list<std::string_view> strings) {
    constexpr_assert(strings.size() == Height,
                     "The number of strings provided must be equal to Height");
    size_t j = 0;
    for (auto str: strings) {
        constexpr_assert(str.size() == Width,
                         "Each string size must be Width");
        size_t i = 0;
        for (auto c: str) {
            at(i++, j) = indexFromChar(c);
        }
        ++j;
    }
}

} // namespace core
