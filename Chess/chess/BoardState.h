#pragma once

#include "../core/Utils.h"

#include "Piece.h"

#include <array>

namespace chess {
//class Piece;
enum class GameResult {
    Continue, Win, Stalemate, Draw
    // WinForWhite,
    // WinForBlack,
    // StalemateForWhite,
    // StalemateForBlack,
    // DrawBy50MoveRule,
    // DrawByThrefoldRepetition,
    // DrawByAgreement,
};
class BoardState {
public:
    BoardState() = default;

    constexpr const Piece*& at(Pos p) { return at(p.x(), p.y()); }
    constexpr const Piece* at(Pos p) const { return at(p.x(), p.y()); }
    constexpr const Piece*& at(int x, int y) { return val[y * 8 + x]; }
    constexpr const Piece* at(int x, int y) const { return val[y * 8 + x]; }

    void reset();

    void update();
    void update(const std::array<std::unique_ptr<Piece>, 64>& pieces);

    bool moveLeavesInCheck(Pos from, Pos to) const;

    GameResult testWinOrStalemate(Side s) const;

    //Forsyth-Edwards Notation
    std::string getFEN() const;

    //like FEN but don't print the conters
    std::string getShortenedFEN() const;

    // outputs as FEN
    friend std::ostream& operator<<(std::ostream& s, const BoardState& b);
    // bool isCheckMate(Side s) const;

    constexpr Pos getEnPassantTarget() const {
        return enPassantTarget;
    }

private:
    std::array<const Piece*, 64> val;
    SideEntries<bool> isInCheck;
    SideEntries<Pos> kingPos;


    // For the 50 move rule
    // Here half move means a player completing their turn
    // Halfmove clock: This is the number of halfmoves since the last capture
    // or pawn advance. This is used to determine if a draw can be claimed
    // under the fifty-move rule.
    int halfMoveClock = 0;

    // it's just used for the FEN
    // It starts at 1, and is incremented after Black's move.
    int moveCounter = 1;
    Side currentSide = Side::White;

    Pos enPassantTarget = Pos::Invalid;

    void incrementHalfMove() {
        ++halfMoveClock;
        if (currentSide != Side::White)
            ++moveCounter;
        currentSide = getOtherSide(currentSide);
    }
    std::ostream& shortenedFenImpl(std::ostream& s) const;

    friend class Board;
};

}//chess
