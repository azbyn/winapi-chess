#pragma once

#include "Utils.h"

#include <iostream>
#include <iomanip>

namespace core {

// Because of the constexpr this everything must be defined in the header file
class Color {
    uint8_t alpha, red, green, blue;
public:
    constexpr uint8_t r() const { return red; }
    constexpr uint8_t g() const { return green; }
    constexpr uint8_t b() const { return blue; }
    constexpr uint8_t a() const { return alpha; }

    constexpr Color() : Color(0,0,0,0) {}
    constexpr Color(uint32_t val)
        : Color(uint8_t(val >> 24), uint8_t(val >> 16),
                uint8_t(val >> 8), uint8_t(val)) {
        if (alpha==0) alpha = 0xFF;
    }

    explicit operator COLORREF() { return RGB(r(), g(), b()); }

    constexpr Color withAlpha(uint8_t a) const {
        return Color(a, red, green, blue);
    }

    constexpr Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
        : alpha(a), red(r), green(g), blue(b) {}

    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : Color(0xFF, r, g, b) {}

    constexpr static Color grayscale(uint8_t v) { return Color(v,v,v); }

    static constexpr uint8_t lerp8(uint8_t a, uint8_t b, uint8_t t) {
        uint16_t res = (255 - t) * a + t * b;
        return res / 255;
    }
    // lerp = Linear intERPolation
    // like the regular lerp, but with values of t between 0 and 255, not 0 to 1
    static constexpr Color lerp8(Color a, Color b, uint8_t t) {
        return Color(
            lerp8(a.r(), b.r(), t),
            lerp8(a.g(), b.g(), t),
            lerp8(a.b(), b.b(), t));
    }
    static const Color Red, Green, Blue,
        NiceRed, NiceYellow, NiceOrange,
        Yellow, Magenta, Cyan,
        Black, White, Gray,
        DarkGray, LightGray,
        Clear, Blurry;

    friend std::ostream& operator<<(std::ostream& s, Color c) {
        s << "#"
          << std::hex << std::setfill('0') << std::setw(2) << (int)c.a()
          << std::hex << std::setfill('0') << std::setw(2) << (int)c.r()
          << std::hex << std::setfill('0') << std::setw(2) << (int)c.g()
          << std::hex << std::setfill('0') << std::setw(2) << (int)c.b();
        return s;
    }
};

inline constexpr Color Color::Red = 0xFF0000;
inline constexpr Color Color::Green = 0x00FF00;
inline constexpr Color Color::Blue = 0x0000FF;

inline constexpr Color Color::NiceRed = 0xCC342B;
inline constexpr Color Color::NiceYellow = 0xFFEB04;
inline constexpr Color Color::NiceOrange = 0xF96A38;

inline constexpr Color Color::Yellow = 0xFFFF00;
inline constexpr Color Color::Magenta = 0xFF00FF;
inline constexpr Color Color::Cyan = 0x00FFFF;

inline constexpr Color Color::Black = 0x000000;
inline constexpr Color Color::White = 0xFFFFFF;
inline constexpr Color Color::Gray = 0x7F7F7F;

inline constexpr Color Color::DarkGray = Color::grayscale(32);
inline constexpr Color Color::LightGray = Color::grayscale(255 - 32);

inline constexpr Color Color::Clear = Color(0, 0, 0, 0);
inline constexpr Color Color::Blurry = Color::Gray.withAlpha(100);

} // namespace core
