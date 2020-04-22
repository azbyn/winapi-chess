#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <stdint.h>

#include <iostream>

#include <stdexcept>
#include <sstream>


#ifndef _MSC_VER
#include <experimental/source_location>
#else
//it's not implemented for the microsoft compiler
namespace std::experimental {
struct source_location {
    static constexpr source_location current() noexcept { return {}; }
    constexpr source_location() noexcept {}
    constexpr uint_least32_t line() const noexcept { return 0; }
    constexpr uint_least32_t column() const noexcept { return 0; }
    constexpr const char* file_name() const noexcept { return "?"; }
    constexpr const char* function_name() const noexcept { return "?"; }
};
}
#endif

namespace core {
template<typename... Args>
std::string concat(Args&&... args) {
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}
constexpr int clamp(int val, int min, int max) {
    if (val < min) return min;
    else if (val > max) return max;
    return val;
}

class WinapiError : public std::runtime_error {
    DWORD code_;
    using source_location = std::experimental::source_location;

public:
    explicit WinapiError(std::string_view msg = "",
                         const source_location& location =
                         source_location::current())
        : std::runtime_error(makeMessage(msg, code_, location)) {}

    virtual ~WinapiError() noexcept = default;

    DWORD code() const { return code_; }

private:
    static std::string makeMessage(std::string_view msg, DWORD& code,
                                   const source_location& location);
};

inline void warning(const char* msg) {
    ::MessageBoxA(nullptr, msg, "Warning", MB_OK);
}
inline void warning(const std::string& msg) {
    warning(msg.c_str());
}

constexpr void constexpr_assert(bool val, const char* msg) {
    if(!val) throw msg;
}
constexpr void constexpr_assert(bool val) {
    constexpr_assert(val, "Assertion failed");
}

constexpr int constexpr_abs(int v) { return v < 0 ? -v : v; }

struct Point {
    int x, y;
    constexpr Point() : x(0), y(0) {}
    constexpr Point(int x, int y) : x(x), y(y) {}

    constexpr static Point lerp(Point p0, Point p1, double t) {
        auto dp = p1 - p0;
        return p0 + Point(int(t*dp.x), int(t * dp.y));
    }

    constexpr double aspectRatio() const { return double(x) / y; }

    constexpr Point& operator+=(Point rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    constexpr Point& operator-=(Point rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    constexpr Point& operator*=(int rhs) {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    constexpr Point& operator/=(int rhs) {
        x /= rhs;
        y /= rhs;
        return *this;
    }
    constexpr friend Point operator+(Point a, Point b) {
        return Point(a.x+b.x, a.y+b.y);
    }
    constexpr friend Point operator-(Point a, Point b) {
        return Point(a.x-b.x, a.y-b.y);
    }
    constexpr Point operator-() const {
        return Point(-x, -y);
    }

    constexpr friend Point operator*(int a, Point b) {
        return Point(a*b.x, a*b.y);
    }
    constexpr friend Point operator*(Point a, int b) {
        return b * a;
    }
    constexpr friend Point operator/(Point a, int b) {
        return Point(a.x/b, a.y/b);
    }

    constexpr friend bool operator==(Point a, Point b) {
        return a.x == b.x && a.y == b.y;
    }

    constexpr friend bool operator!=(Point a, Point b) {
        return a.x != b.x || a.y != b.y;
    }
    constexpr Point abs() const {
        return Point(constexpr_abs(x), constexpr_abs(y));
    }
    constexpr struct Rect asRect() const;

    friend std::ostream& operator<<(std::ostream& s, Point p) {
        return s << "(" << p.x << "," << p.y <<")";
    }
    constexpr int64_t length2() const {
        return x * x + y * y;
    }
};

struct Rect {
    int left, top, right, bottom;

    constexpr Rect(): Rect(0,0,0,0) {}
    constexpr Rect(int left, int top, int right, int bottom)
        : left(left), top(top), right(right), bottom(bottom) {}

    constexpr Rect(Point topLeft, Point botRight)
        : Rect(topLeft.x, topLeft.y, botRight.x, botRight.y) {}
    constexpr Rect(RECT val)
        : Rect(val.left, val.top, val.right, val.bottom) {}

    static constexpr Rect fromMiddleAndSize(Point middle, Point size) {
        Point halfSize = size/2;
        return Rect(middle - halfSize, middle+halfSize);
    }

    constexpr operator RECT() const {
        return {left, top, right, bottom};
    }

    constexpr int width() const { return right - left; }
    constexpr int height() const { return bottom - top; }

    constexpr Point topLeft() const { return {left, top}; }
    constexpr Point topRight() const { return {right, top}; }
    constexpr Point bottomLeft() const { return {left, bottom}; }
    constexpr Point bottomRight() const { return {right, bottom}; }

    constexpr Point p0() const { return {left, top}; }
    constexpr Point p1() const { return {right, bottom}; }

    constexpr Point middle() const { return (p0()+p1())/2;}

    constexpr int& x0() { return left; }
    constexpr int& y0() { return top; }

    constexpr int& x1() { return right; }
    constexpr int& y1() { return bottom; }

    constexpr int x0() const { return left; }
    constexpr int y0() const { return top; }

    constexpr int x1() const { return right; }
    constexpr int y1() const { return bottom; }

    constexpr Point size() const { return {width(), height()}; }

    friend constexpr Rect operator+(Point p, Rect r) {
        return Rect(r.x0() + p.x, r.y0() + p.y,
                    r.x1() + p.x, r.y1() + p.y);
    }

    friend constexpr Rect operator+(Rect r, Point p) { return p + r; }
    friend constexpr Rect operator-(Rect r, Point p) { return r + (-p); }

    constexpr Rect& operator+=(Point p) {
        x0() += p.x;
        y0() += p.y;
        x1() += p.x;
        y1() += p.y;
        return *this;
    }

    constexpr Rect& operator-=(Point p) {
        return *this += -p;
    }

    constexpr bool contains(Point p) const {
        return p.x >= x0() && p.x <= x1() &&
            p.y >= y0() && p.y <= y1();
    }
    static Rect getClientRect(HWND hWnd) {
        RECT r;
        if (!::GetClientRect(hWnd, &r))
            throw WinapiError("GetClientRect");
        return r;
    }

    void adjustWindowRect(DWORD dwStyle, BOOL bMenu) {
        RECT val = *this;
        if (!::AdjustWindowRect(&val, dwStyle, bMenu)) {
            throw WinapiError("AdjustWindowRect");
        }
        new (this) Rect(val);
    }

    friend std::ostream& operator <<(std::ostream& s, const Rect& r) {
        return s << "(" << r.left << ", " << r.top
                 << ", " << r.right << ", " << r.bottom << ")";
    }
};


constexpr Rect Point::asRect() const { return {{0,0}, *this}; }

class WindowDC {
    HDC val;
    HWND hwnd;
public:

    WindowDC(HWND hwnd)
        : val(::GetDC(hwnd)),
          hwnd(hwnd) {
        if (val == nullptr)
            throw WinapiError("GetDC");
    }
    WindowDC(const WindowDC&) = delete;
    WindowDC(WindowDC&&) = delete;
    WindowDC& operator=(const WindowDC&) = delete;
    WindowDC& operator=(WindowDC&&) = delete;

    operator HDC() const { return val; }
    ~WindowDC() noexcept {
        ::ReleaseDC(hwnd, val);
    }
};

} // namespace core
