#include "Board.h"

#include "Piece.h"


namespace chess {
template <class T>
void addBoth(Board* p, int x) {
    p->at(x, 0) = std::make_unique<T>(Side::White);
    p->at(x, 7) = std::make_unique<T>(Side::Black);
}

void Board::reset() {
    pieces = {};
    for (auto& vec : eatenPieces)
        vec.clear();
    state.reset();

    for (int i = 0; i < 8; ++i) {
        at(i, 1) = std::make_unique<Pawn>(Side::White);
        at(i, 6) = std::make_unique<Pawn>(Side::Black);
    }
    addBoth<Rook>(this, 0);
    addBoth<Rook>(this, 7);

    addBoth<Knight>(this, 1);
    addBoth<Knight>(this, 6);

    addBoth<Bishop>(this, 2);
    addBoth<Bishop>(this, 5);

    addBoth<Queen>(this, 3);
    addBoth<King>(this, 4);
    // at(4, 7) = std::make_unique<King>(Side::Black);
    // at(0, 7) = std::make_unique<Rook>(Side::Black);
    // for (int i = 0; i < 4; ++i)
    //     at(i, 6) = std::make_unique<Pawn>(Side::Black);

    state.update(pieces);
}
Board::Board(PromotionCallback promotionCallback,
             CheckmateCallback checkmateCallback,
             StalemateCallback stalemateCallback,
             DrawCallback      drawCallback)
        : pieces{},
          promotionCallback(promotionCallback),
          moveExecutedCallback(nullptr),
          checkmateCallback(checkmateCallback),
          stalemateCallback(stalemateCallback),
          drawCallback(drawCallback) {}

void Board::finishMove(FullMove move) {
    state.update(pieces);

    for (auto side : {Side::White, Side::Black}) {
        if (!state.isInCheck[side]) continue;
        auto res = state.testWinOrStalemate(side);

        switch (res) {
        case GameResult::Win:
            checkmateCallback(move, getOtherSide(side));
            break;
        case GameResult::Stalemate:
            stalemateCallback(move, side);
            break;
        default:
            break;
        }
    }
    std::cout << state << "\n";
    // 50 half moves = 100 moves
    if (state.halfMoveClock >= 100)
        drawCallback(move, "Fifty-move rule");

    // auto shortenedFEN = state.getShortenedFEN();
    
    auto count = ++boardHistory[state.getShortenedFEN()];

    std::cout << "count: "<< count<< "\n";
    if (count == 3) {
        drawCallback(move, "Threefold repetition");
    }
    moveHistory.push_back(move);
    state.incrementHalfMove();
}

void Board::eatAt(Pos p) {
    auto t = std::exchange(at(p), nullptr);
    if (t != nullptr) {
        state.halfMoveClock = 0;

        eatenPieces[t->getSide()].push_back(std::move(t));
    }
}

void Board::onGetPromotionResult(Side side, PromotionResult res) {
    std::cout << "onGetPromotionResult( "<< side
              << " vs " << getCurrentSide() << "\n";
    auto& to = at(promotionMove.to);
    switch (res) {
    case PromotionResult::Knight: to = std::make_unique<Knight>(side); break;
    case PromotionResult::Bishop: to = std::make_unique<Bishop>(side); break;
    case PromotionResult::Rook:   to = std::make_unique<Rook>(side); break;
    case PromotionResult::Queen:  to = std::make_unique<Queen>(side); break;
    default:
        std::cerr << "PromotionResult = " << (int) res << "\n";
        throw std::logic_error("invalid promotionResult");
    }
    // previouslyMovedPiece = at(promotionMove.to).get();
    promotionMove.promotionResult = res;
    //TODO DOES THIS STILL WORK?
    finishMove(promotionMove);
    if (moveExecutedCallback) {
        moveExecutedCallback(promotionMove);
    }
    
    // promotionMove.to = {-1, -1};
}
void Board::moveUnchecked(Pos from, Pos to) {
    // we don't need to check for null, dynamic_cast already does that
    // if (auto* pawn = dynamic_cast<Pawn*>(previouslyMovedPiece)) {
    //     // std::clog << "previous was pawn\n";
    //     pawn->setCanBeEnPassanted(false);
    // }

    eatAt(to);
    at(to) = std::exchange(at(from), nullptr);
    at(to)->onMoved();

    // if (auto* king = dynamic_cast<King*>(at(to).get())) {
    //     kingPos[king->getSide()] = to;
    // }
    // previouslyMovedPiece = at(to).get();
}
void doNothingPC(Side) {}

void Board::doFullMove(FullMove move) {
    std::cout << "dfm" << move << "\n";
    auto pc = promotionCallback;
    promotionCallback = doNothingPC;

    auto doFirstValid = [&] () {
          std::vector<Move> validMoves;
          for (int j = 0; j < 8; ++j) {
              for (int i = 0; i < 8; ++i) {
                  auto& ptr = at(i, j);
                  if (ptr == nullptr) continue;
                  if (ptr->getSide() != getCurrentSide()) continue;
                  ptr->getValidMoves(move.from, state, validMoves);
                  if (validMoves.empty()) continue;
                  auto p = validMoves.front().pos;
                  if (tryMove({i, j}, p, nullptr)) {
                      finishMove({{i, i}, p});
                      std::cout << "OI\n";
                      return;
                  }
              }
          }
      };

    if (!tryMove(move.from, move.to, nullptr)) {
        std::cout << "fen " << state << "\n";
        std::cerr <<("Can't do that?! Doing first valid move.\n");
        doFirstValid();
    }
    if (move.promotionResult != PromotionResult::None) {
        onGetPromotionResult(getCurrentSide(), move.promotionResult);
    }

    promotionCallback = pc;
}

bool Board::tryMove(Pos from, Pos to, MoveExecutedCallback callback) {
    std::cout << "tryMove: "<< from << " to " << to << "\n";
    auto& ptr = at(from);
    if (ptr == nullptr)
        return false;

    if (dynamic_cast<const King*>(ptr.get())) {
        if (ptr->getSide() == Side::Black) {
            std::cout << "KING first: " << ptr->getMadeFirstMove() << "\n";
        }
    }

    std::vector<Move> validMoves;
    ptr->getValidMoves(from, state, validMoves);

    auto it = std::find_if(validMoves.begin(), validMoves.end(),
                           [&](Move m) { return m.pos == to; });
    if (it == validMoves.end())
        return false;

    FullMove move {from, to};

    if (dynamic_cast<const Pawn*>(ptr.get()) != nullptr)
        state.halfMoveClock = 0;

    if (it->type != Move::Type::EnPassant)
        state.enPassantTarget = Pos::Invalid;

    switch (it->type) {
    case Move::Type::DoubleAdvance: {
        state.enPassantTarget = to;
    } break;
    case Move::Type::EnPassant: {
        eatAt(state.enPassantTarget);
    } break;
    case Move::Type::Promotion:
        promotionMove = move;
        this->moveExecutedCallback = callback;
        promotionCallback(state.currentSide);
        break;
    case Move::Type::QueensideCastling:
        moveUnchecked({0, from.y()}, from-Pos{1, 0});
        break;
    case Move::Type::Castling:
        moveUnchecked({7, from.y()}, from+Pos{1, 0});
        break;
    case Move::Type::Normal:
        break;
    }

    moveUnchecked(from, to);

    if (it->type != Move::Type::Promotion) {
        finishMove(move);
        if (callback)
            callback(move);
    }

    return true;
}

} // namespace chess
