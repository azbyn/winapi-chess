#pragma once

#include "Utils.h"
#include "Color.h"
#include "Sprite.h"

#include <array>
#include <vector>
#include <string_view>

namespace core {

namespace textFormat {

constexpr static auto Center =
    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;

constexpr static auto MultilineCenter =
    DT_CENTER | DT_NOCLIP;
}

//first color is reserved for transparency
template<size_t size>
using Palette = std::array<Color, size>;


class Paint {
public:
    //only to be used in WindowHandler
    class StretchData {
    public:
        // We don't use an actual rect because ::StretchBlt takes
        // a starting position and a size, and we call those functions
        // once per frame.
        struct SzRect {
            int x, y, w, h;

            constexpr SzRect() : x(0), y(0), w(0), h(0) {}
            constexpr SzRect(int x, int y, int w, int h)
                : x(x), y(y), w(w), h(h) {}

            // constexpr SzRect(Rect r)
            //     : x(r.x0()), y(r.y0()), w(r.width()), h(r.height()) {}

            // constexpr operator Rect() const {
            //     return {x, y, x+w, y+h};
            // }
            constexpr Point p0() const { return {x, y}; }
            constexpr int x1() const { return x + w; }
            constexpr int y1() const { return y + h; }
        };

        StretchData(Point size, bool shouldVal);

        void updateSize(Point dstSize);

        Point dstToSrcCoord(Point dstPt);

        void setShouldStretch(bool val);

    private:
        double dstToSrcRatio;
        bool shouldStretch;
        //we cache those
        double srcAspectRatio;
        Point srcSize; //, dstSize;

        std::array<SzRect, 2> margins;
        SzRect drawRect;

        // double dstAR;

        // Point marginSize;
        // Point secondMarginPoint;


        // Point size;
        // Point offset;

        friend class Paint;
    };
    Paint(HDC ogHDC, Point size);
    ~Paint() noexcept;

    Paint(const Paint&) = delete;
    Paint(Paint&&) = delete;
    Paint& operator=(const Paint&) = delete;
    Paint& operator=(Paint&&) = delete;

    constexpr int width() const { return bmi.bmiHeader.biWidth; }
    constexpr int height() const { return bmi.bmiHeader.biHeight; }

    constexpr Point size() const { return {width(), height()}; }

    void setPixel(Point p, Color col);
    void setPixel(int x, int y, Color col);

    void copyTo(HDC dst) const;
    void copyToStretched(HDC dst, const StretchData& stretchData) const;
    void copyTo(Paint& dst) const;

    /* because textOut(x, y,s) calls textOut(p, s);
       it's better to not use a const Point&

       less assembly instructions:
       https://godbolt.org/z/7dghsf
    */
    void textOut(Point p, std::string_view s);
    void textOut(int x, int y, std::string_view s);

    constexpr static auto CenterTextFormat =
        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP;
    void drawText(Rect r, std::string_view s, UINT format = textFormat::Center);

    void setFont(const char* name, int size, bool bold = false);
    void setTextColor(Color c);


    void fillRect(int x0, int y0, int x1, int y1, Color col);
    void fillRect(Rect r, Color col);

    //draws a border inside r
    void drawRectIn(Rect r, int thickness, Color col);

    //draws a border outside r
    void drawRectOut(Rect r, int thickness, Color col);

    void fill(Color col);

    void fillCircle(Point center, int radius, Color col);
    //A purposefully pixelated circle looks better than one without antialiasing
    void fillPixelatedCircle(Point center, int radius, Color col,
                             int pixelSize);

    template<size_t Width, size_t Height, const char CharPalette_[],
             size_t PaletteSize>
    void drawSprite(int x, int y,
                    const PaletteSprite<Width, Height, CharPalette_>& s,
                    const Palette<PaletteSize>& palette);

    template<size_t Width, size_t Height, const char CharPalette_[],
             size_t PaletteSize>
    void drawSprite(int x, int y,
                    const PaletteSprite<Width, Height, CharPalette_>& s,
                    const Palette<PaletteSize>& palette,
                    int scale);


    template<size_t Width, size_t Height, const char CharPalette_[],
             size_t PaletteSize>
    void drawSprite(Point p,
                    const PaletteSprite<Width, Height, CharPalette_>& s,
                    const Palette<PaletteSize>& palette);

    template<size_t Width, size_t Height, const char CharPalette_[],
             size_t PaletteSize>
    void drawSprite(Point p,
                    const PaletteSprite<Width, Height, CharPalette_>& s,
                    const Palette<PaletteSize>& palette,
                    int scale);
    void getSnapshot(std::vector<uint8_t>& snap) const;
    void setSnapshot(const std::vector<uint8_t>& snap);

private:
    BITMAPINFO bmi;
    uint8_t* data;
    HDC hdc;
    HBITMAP bitmap;
    HBITMAP oldBitmap;
    HFONT oldFont;
    Rect bounds;

    constexpr int byteSize() const { return bmi.bmiHeader.biSizeImage; }
    HFONT setFontImpl(const char* name, int size, bool bold);

    constexpr void setPixelUnchecked(uint8_t* ptr, Color col);
    constexpr void setPixelUnchecked(int x, int y, Color col);
};

constexpr void Paint::setPixelUnchecked(int x, int y, Color col) {
    auto at = [this] (int x, int y) {
        return data + (y*width()+x)*3;
    };
    setPixelUnchecked(at(x, y), col);
}
constexpr void Paint::setPixelUnchecked(uint8_t* ptr, Color col) {
    constexpr auto setColor = [](uint8_t* ptr, Color col) {
        ptr[2] = col.r();
        ptr[1] = col.g();
        ptr[0] = col.b();
    };
    constexpr auto getColor = [] (const uint8_t* ptr) {
        return Color(ptr[2], ptr[1], ptr[0]);
    };
    Color c = getColor(ptr);
    setColor(ptr, Color::lerp8(c, col, col.a()));
}

template<size_t W, size_t H, const char CP[], size_t PSize>
void Paint::drawSprite(Point p, const PaletteSprite<W, H, CP>& s,
                       const Palette<PSize>& palette) {
    for (size_t j = 0; j < H; ++j) {
        for (size_t i = 0; i < W; ++i) {
            auto index = s(i, j);
            if (index) setPixel(p + Point(i, j), palette[index]);
        }
    }
}

template<size_t W, size_t H, const char CP[], size_t PSize>
void Paint::drawSprite(Point p,
                       const PaletteSprite<W, H, CP>& s,
                       const Palette<PSize>& palette,
                       int scale) {
    for (size_t j = 0; j < H; ++j) {
        for (size_t i = 0; i < W; ++i) {
            auto index = s(i, j);
            if (!index)
                continue;
            auto start = Point(i, j) * scale + p;
            fillRect({start, start + Point(scale, scale)}, palette[index]);
        }
    }
}

template<size_t W, size_t H, const char CP[], size_t PSize>
void Paint::drawSprite(int x, int y,
                       const PaletteSprite<W, H, CP>& s,
                       const Palette<PSize>& palette) {
    return drawSprite({x, y}, s, palette);
}

template<size_t W, size_t H, const char CP[], size_t PSize>
void Paint::drawSprite(int x, int y,
                       const PaletteSprite<W, H, CP>& s,
                       const Palette<PSize>& palette,
                       int scale) {
    return drawSprite({x, y}, s, palette, scale);
}

} // namespace core
