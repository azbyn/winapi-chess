#pragma once

#include "../core/Utils.h"
#include "Piece.h"
#include "BoardState.h"

#include <memory>
#include <vector>
#include <map>

namespace chess {
class Piece;
class Board {
public:
    using PromotionCallback = void(*)(Side);
    using CheckmateCallback = void(*)(FullMove, Side whoWon);
    using StalemateCallback = void(*)(FullMove, Side whoCantMove);
    using DrawCallback      = void(*)(FullMove, std::string_view why);
    using MoveExecutedCallback = void(*)(FullMove);

    Board(PromotionCallback promotionCallback,
          CheckmateCallback checkmateCallback,
          StalemateCallback stalemateCallback,
          DrawCallback      drawCallback);

    Board(const Board&) = delete;
    Board(Board&&) = delete;
    Board& operator=(const Board&) = delete;
    Board& operator=(Board&&) = delete;

    void reset();

    void onGetPromotionResult(Side side, PromotionResult res);

    constexpr std::unique_ptr<Piece>& at(int x, int y) {
        return pieces[y * 8 + x];
    }
    constexpr const std::unique_ptr<Piece>& at(int x, int y) const {
        return pieces[y * 8 + x];
    }
    constexpr std::unique_ptr<Piece>& at(Pos p) { return at(p.x(), p.y()); }
    constexpr const std::unique_ptr<Piece>& at(Pos p) const {
        return at(p.x(), p.y());
    }

    // Returns true if move is valid and executes it
    // moveExecutedCallback can be null
    bool tryMove(Pos from, Pos to, MoveExecutedCallback moveExecutedCallback);

    // There's no try, only do
    void doFullMove(FullMove move);

    constexpr Side getCurrentSide() const { return state.currentSide; }

    constexpr bool getIsInCheck(Side side) const {
        return state.isInCheck[side];
    }

    const auto& getState() const { return state; }
    const auto& getMoveHistory() const { return moveHistory; }

    constexpr const auto& getEatenPieces(Side side) const {
        return eatenPieces[side]; }


private:
    std::array<std::unique_ptr<Piece>, 64> pieces;
    BoardState state;

    // key: board state in FEN
    // value: how many times it appears
    std::map<std::string, int> boardHistory;


    SideEntries<std::vector<std::unique_ptr<Piece>>> eatenPieces;

    std::vector<FullMove> moveHistory;

    PromotionCallback promotionCallback;
    FullMove promotionMove;

    // Stored just for promotion (the move is executed after we get the
    // promotion result)
    MoveExecutedCallback moveExecutedCallback;

    CheckmateCallback checkmateCallback;
    StalemateCallback stalemateCallback;
    DrawCallback      drawCallback;

    void eatAt(Pos p);
    void moveUnchecked(Pos from, Pos to);
    void finishMove(FullMove move);
};

} // namespace chess
