#pragma once

#include "Utils.h"
#include "Color.h"
#include "PaletteSprite.h"

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

class Paint {
public:
    // Only to be used in WindowHandler
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

        double srcAspectRatio;
        Point srcSize;

        std::array<SzRect, 2> margins;
        SzRect drawRect;

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

    // Draws a border inside r
    void drawRectIn(Rect r, int thickness, Color col);

    // Draws a border outside r
    void drawRectOut(Rect r, int thickness, Color col);

    void fill(Color col);

    void fillCircle(Point center, int radius, Color col);
    // A purposefully pixelated circle looks better than one without antialiasing
    void fillPixelatedCircle(Point center, int radius, Color col,
                             int pixelSize);

    void drawSprite(int x, int y,
                    const PaletteSprite& sprite,
                    const Palette& palette);

    void drawSprite(int x, int y,
                    const PaletteSprite& sprite,
                    const Palette& palette,
                    int scale);

    void drawSprite(Point p,
                    const PaletteSprite& sprite,
                    const Palette& palette);

    void drawSprite(Point p,
                    const PaletteSprite& s,
                    const Palette& palette,
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

    void setPixelUnchecked(uint8_t* ptr, Color col);
    void setPixelUnchecked(int x, int y, Color col);
};

} // namespace core
