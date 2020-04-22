#pragma once

#include "../Sprites.h"
#include "Common.h"

#include <algorithm>
#include <vector>

namespace chess {

class Board;
class BoardState;
class Piece {
protected:
    using Sprite = core::PaletteSprite;
    struct ValidMovesHandler {
        const BoardState& b;
        std::vector<Move>& res;
        Pos pos;
        Side side;

        void add(Pos pos, Move::Type type = Move::Type::Normal);

        // Doesn't add if the piece has the same side as the param
        // returns true if we should stop (when going in a direction)
        bool tryAdd(Pos pos);

        void addDiagonals();
        void addHorizontalAndVertical();
    };

public:
    constexpr Piece(Side side) : side(side) {}

    virtual ~Piece() noexcept = default;

    constexpr Side getSide() const { return side; }
    constexpr bool getMadeFirstMove() const { return madeFirstMove; }


    virtual const Sprite& getSprite() const = 0;

    constexpr static const auto& getPalette(Side side) {
        if (side == Side::White)
            return sprites::WhitePalette;
        return sprites::BlackPalette;
    }

    constexpr const auto& getPalette() const { return getPalette(side); }

    void getValidMoves(Pos pos, const BoardState& b,
                       std::vector<Move>& res) const;
    void getValidMovesDontTestCheck(Pos pos, const BoardState& b,
                                    std::vector<Move>& res) const;

    void onMoved() { madeFirstMove = true; }

    char getLetter() const {
        char l = getLetterImpl();
        return side == Side::White ? toupper(l) : tolower(l);
    }

protected:
    virtual void getValidMoves(ValidMovesHandler vmh) const = 0;
    // Shouldn't care about the side of the piece
    virtual char getLetterImpl() const = 0;

private:
    Side side;
    bool madeFirstMove = false;

};

class King : public Piece {
public:
    constexpr King(Side side) : Piece(side) {}
    const Sprite& getSprite() const override { return sprites::King; }
    char getLetterImpl() const override { return 'K'; }

protected:
    void getValidMoves(ValidMovesHandler vmh) const override;
};

class Queen : public Piece {
public:
    constexpr Queen(Side side) : Piece(side) {}
    const Sprite& getSprite() const override { return sprites::Queen; }
    char getLetterImpl() const override { return 'Q'; }

protected:
    void getValidMoves(ValidMovesHandler vmh) const override {
        vmh.addDiagonals();
        vmh.addHorizontalAndVertical();
    }
};

class Rook : public Piece {
public:
    constexpr Rook(Side side) : Piece(side) {}
    const Sprite& getSprite() const override { return sprites::Rook; }
    char getLetterImpl() const override { return 'R'; }

protected:
    void getValidMoves(ValidMovesHandler vmh) const override {
        vmh.addHorizontalAndVertical();
    }
};

class Bishop : public Piece {
public:
    constexpr Bishop(Side side) : Piece(side) {}
    const Sprite& getSprite() const override { return sprites::Bishop; }
    char getLetterImpl() const override { return 'B'; }

protected:
    void getValidMoves(ValidMovesHandler vmh) const override {
        vmh.addDiagonals();
    }
};

class Knight : public Piece {
public:
    constexpr Knight(Side side) : Piece(side) {}
    const Sprite& getSprite() const override { return sprites::Knight; }
    char getLetterImpl() const override { return 'N'; }

protected:
    void getValidMoves(ValidMovesHandler vmh) const override;
};

class Pawn : public Piece {
public:
    constexpr Pawn(Side side) : Piece(side) {}
    const Sprite& getSprite() const override { return sprites::Pawn; }
    char getLetterImpl() const override { return 'P'; }

protected:
    void getValidMoves(ValidMovesHandler vmh) const override;
};

} //namespace chess
