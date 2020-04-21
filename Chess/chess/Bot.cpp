#include "Bot.h"

#include "BoardState.h"

#include "../stockfish/endgame.h"
#include "../stockfish/thread.h"
#include "../stockfish/uci.h"


// #include "../stockfish/bitboard.h"
// #include "../stockfish/position.h"
// #include "../stockfish/tt.h"
// #include "../stockfish/syzygy/tbprobe.h"

#include <sstream>


// FEN string of the initial position, normal chess
constexpr static const char* StartFEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


namespace stockfish::PSQT {
void init();
}

constexpr chess::Pos squareToPos(stockfish::Square s) {
    using namespace stockfish;
    File f = file_of(s);
    Rank r = rank_of(s);
    return {f, r};
}
constexpr chess::PromotionResult toPR(stockfish::PieceType pt) {
    using PR = chess::PromotionResult;
    switch (pt) {
    case stockfish::KNIGHT: return PR::Knight;
    case stockfish::BISHOP: return PR::Bishop;
    case stockfish::ROOK:   return PR::Rook;
    case stockfish::QUEEN:  return PR::Queen;
    default:                return PR::None;
    }
}

namespace stockfish::Search {
void onBestMoveFound(stockfish::Move sm) {
    chess::Pos from = squareToPos(from_sq(sm));
    chess::Pos to = squareToPos(to_sq(sm));
    chess::PromotionResult pr = toPR(promotion_type(sm));

    MoveType mt = type_of(sm);
    if (mt != PROMOTION) pr = chess::PromotionResult::None;

    if (mt == CASTLING) {
        std::cout << "CASTLING\n";
        if (from.x() > to.x()) {
            to.x() = from.x() - 2;
            std::cout << "king side ";
        } else {
            to.x() = from.x() + 2;
            std::cout << "queen side ";
        }
        std::cout << "CASTLING: \n";
    }
    chess::FullMove move(from, to, pr);

    std::cout << "Found move: "<< move << "\n";
    std::cout << "move type: "<< mt << "\n";
    chess::Bot::instance->foundMoveCallback(move);
    // chess::Bot::instance->doMove(move);
}
}

using namespace stockfish;

namespace chess {

int Bot::difficulty = 3;
Bot* Bot::instance = nullptr;
stockfish::Search::LimitsType Bot::limits;

class InvalidMoveError {};
//from UCI::to_move
stockfish::Move Bot::toStockfishMove(const FullMove& m) {
    char buff[6] = {};
    m.writeStringAt(buff);

    std::string_view str = buff;

    for (const auto& mov : MoveList<LEGAL>(pos)) {
        if (str == UCI::move(mov, false))// pos.is_chess960()))
            return mov;
    }

    throw InvalidMoveError{};
}

Bot::Bot(FoundMoveCallback callback, int depth, int64_t nodes,
          std::chrono::milliseconds::rep timeMs)
        : foundMoveCallback(callback) {
    if (instance != nullptr) {
        throw new std::logic_error("can't have multiple Bot instances");
    }
    instance = this;
    limits.depth = depth;
    limits.nodes = nodes;
    limits.time[WHITE] = limits.time[BLACK] = timeMs;
    // limits.movetime = 4000;

    UCI::init(Options);
    PSQT::init();
    Bitboards::init();
    Position::init();
    Bitbases::init();
    Endgames::init();
    Threads.set(Options["Threads"]);
    Search::clear();
    // reset();
}

void Bot::setDifficulty(int val) {
    val = core::clamp(val, MinDifficulty, MaxDifficulty);
    difficulty = val;
    std::cout  << "diff "<< difficulty <<"\n";
    // auto& limits = limits;
    if (val == 0) {
        limits.depth = 1;
        limits.nodes = 5;
    } else if (val == 1) {
        limits.depth = 1;
        limits.nodes = 10;
    } else if (val == 2) {
        limits.depth = 1;
        limits.nodes = 50;
    } else {
        limits.depth = val-2;
    }
}
int Bot::getDifficulty() {
    return difficulty;
}
void Bot::reset() {
    Search::clear();
    states = std::make_unique<std::deque<StateInfo>>(1);
    pos.set(StartFEN, false, &states->back(), Threads.main());
}
// void Bot::print() {
//     std::cout << pos << "\n";
// }
void Bot::stop() {
    Threads.stop = true;
}

void Bot::onPlayerMove(const BoardState& state) {
    states = std::make_unique<std::deque<StateInfo>>(1);
    pos.set(state.getFEN(), false, &states->back(), Threads.main());
    // doMove(m);
    // print();
    think();
}
//from uci::doMove
void Bot::doMove(FullMove m) {
    // std::cout << "doMove (" << m << ")\n";
    try {
        // Drop old and create a new one
        states = StateListPtr(new std::deque<StateInfo>(1));
        states->emplace_back();
        pos.do_move(toStockfishMove(m), states->back());
    } catch (const InvalidMoveError&) {
        std::stringstream ss;
        ss << "Move considered invalid by stockfish: ";
        ss << m;
        ss << ". Ignoring.";
        core::warning(ss.str());
    }
}
void Bot::think() {
    bool ponderMode = false;
    Threads.start_thinking(pos, states, limits, ponderMode);
}

Bot::~Bot() noexcept {
    stockfish::Threads.set(0);
}

}
