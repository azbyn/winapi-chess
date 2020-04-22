#pragma once

#include "../core/Utils.h"

#include <array>

namespace chess {
struct Pos {
    core::Point val;
    constexpr Pos() : val() {}
    constexpr Pos(int x, int y) : val(x, y) {}
    explicit constexpr Pos(core::Point p) : val(p) {}
    constexpr explicit operator core::Point() const { return val; }

    constexpr int& x() { return val.x; }
    constexpr int& y() { return val.y; }
    constexpr int x() const { return val.x; }
    constexpr int y() const { return val.y; }

    constexpr Pos operator+(Pos p) const { return Pos(val + p.val); }
    constexpr Pos operator-(Pos p) const { return Pos(val - p.val); }
    constexpr core::Point operator*(int i) const { return val*i; }

    friend bool operator ==(Pos a, Pos b) { return a.val == b.val; }
    friend bool operator !=(Pos a, Pos b) { return a.val != b.val; }

    friend std::ostream& operator<<(std::ostream& s, Pos p) {
        if (!p.isValid()) return s << '-';
        char buff[3] = {};
        p.writeStringAt(buff);
        return s << buff;
    }

    constexpr bool isValid() const {
        return x() >= 0 && x() < 8 && y() >= 0 && y() < 8;
    }
    // Doesn't take care of invalid
    constexpr void writeStringAt(char* c) const {
        c[0] = 'a' + x();
        c[1] = '1' + y();
    };

    // You shouldn't check == Invalid. Use isValid() instead.
    static const Pos Invalid;
};
inline constexpr Pos Pos::Invalid = {-1, -1};

struct Move {
    enum class Type {
        Normal,
        // When a pawn moves two squares forward it can be eaten by en passant
        DoubleAdvance,
        EnPassant,
        Castling,
        QueensideCastling,
        Promotion,
    };
    Pos pos;
    Type type;
    constexpr Move(Pos pos, Type type) : pos(pos), type(type) {}
};

enum class PromotionResult : char {
    None   = 0,
    Knight = 'n',
    Bishop = 'b',
    Rook   = 'r',
    Queen  = 'q',
};


struct FullMove {
    Pos from, to;
    PromotionResult promotionResult;

    constexpr FullMove() : FullMove(Pos::Invalid, Pos::Invalid, {}) {}
    constexpr FullMove(Pos from, Pos to,
                       PromotionResult promotionResult = PromotionResult::None)
            : from(from), to(to), promotionResult(promotionResult) {}

    constexpr void writeStringAt(char* p) const {
        from.writeStringAt(p);
        to.writeStringAt(p+2);
        p[4] = (char) promotionResult;
    };
    friend std::ostream& operator<<(std::ostream& s, const FullMove& m) {
        char buff[6] = {};
        m.writeStringAt(buff);
        return s << buff;
    }
};

enum class Side { White = 0, Black = 1 };
inline std::ostream& operator<<(std::ostream& s, Side side) {
    switch (side) {
    case Side::White: return s << "White";
    case Side::Black: return s << "Black";
    }
    return s << "?";
}
constexpr Side getOtherSide(Side s) {
    return (Side) (!(bool)s);
}
template<typename T>
class SideEntries {
    std::array<T, 2> val;

public:
    constexpr auto begin() { return val.begin(); }
    constexpr auto begin() const { return val.begin(); }
    constexpr auto end() { return val.end(); }
    constexpr auto end() const { return val.end(); }

    constexpr T& operator[](Side side) { return val[(int)side]; }
    constexpr const T& operator[](Side side) const { return val[(int)side]; }
};

}
