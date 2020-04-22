#pragma once

#include "../stockfish/search.h"

#include <chrono>

#include "Piece.h"

#include <iostream>
#include <string>

namespace chess {
class Bot {
public:
    constexpr static int MinDifficulty = 0;
    constexpr static int MaxDifficulty = 10;
    static void setDifficulty(int val);
    static int  getDifficulty();
    using FoundMoveCallback = void(*)(FullMove m);

    Bot(FoundMoveCallback callback,
        int depth = 0, int64_t nodes = 0,
        std::chrono::milliseconds::rep timeMs = 200);
    ~Bot() noexcept;

    void onPlayerMove(const BoardState& state);
    void reset();
    void stop();

private:
    stockfish::Position pos;
    stockfish::StateListPtr states;
    static stockfish::Search::LimitsType limits;

    stockfish::Move toStockfishMove(const FullMove& m);
    FoundMoveCallback foundMoveCallback;

    // Must be static, getDifficulty might get called before this initializes
    static int difficulty;
    static Bot* instance;
    void doMove(FullMove m);

    friend void ::stockfish::Search::onBestMoveFound(stockfish::Move move);

    void think();
};
} // namespace chess
