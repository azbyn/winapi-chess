#include "Piece.h"

#include "Board.h"

namespace chess {

void Piece::ValidMovesHandler::add(Pos pos, Move::Type type) {
    res.emplace_back(pos, type);
}

bool Piece::ValidMovesHandler::tryAdd(Pos pos) {
    if (!pos.isValid())
        return true;

    auto* ptr = b.at(pos);
    if (ptr && ptr->getSide() == side)
        return true;

    add(pos);
    return ptr != nullptr;
}

void Piece::ValidMovesHandler::addDiagonals() {
    for (int i = 1; i < 8; ++i) {
        if (tryAdd(pos + Pos(i,i))) break;
    }

    for (int i = 1; i < 8; ++i) {
        if (tryAdd(pos - Pos(i,i))) break;
    }

    for (int i = 1; i < 8; ++i) {
        if (tryAdd(pos + Pos(-i,i))) break;
    }

    for (int i = 1; i < 8; ++i) {
        if (tryAdd(pos + Pos(i,-i))) break;
    }
}
void Piece::ValidMovesHandler::addHorizontalAndVertical() {
    for (int i = pos.x()+1; i < 8; ++i) {
        if (tryAdd({i, pos.y()})) break;
    }
    for (int i = pos.x()-1; i >= 0; --i) {
        if (tryAdd({i, pos.y()})) break;
    }

    for (int i = pos.y()+1; i < 8; ++i) {
        if (tryAdd({pos.x(), i})) break;
    }
    for (int i = pos.y()-1; i >= 0; --i) {
        if (tryAdd({pos.x(), i})) break;
    }
}


void Piece::getValidMoves(Pos pos, const BoardState& state,
                          std::vector<Move>& validMoves) const {
    getValidMovesDontTestCheck(pos, state, validMoves);
    std::vector<Move> res;
    for (auto m : validMoves) {
        if (!state.moveLeavesInCheck(pos, m.pos))
            res.push_back(m);
    }
    validMoves = res;
}
void Piece::getValidMovesDontTestCheck(Pos pos, const BoardState& b,
                                       std::vector<Move>& res) const {
    res.clear();
    getValidMoves({b, res, pos, side});
}

void King::getValidMoves(ValidMovesHandler vmh) const {
    auto tryAddCastling = [&] (int x, int direction, auto type) {
        auto* ptr = vmh.b.at(x, vmh.pos.y());
        if (auto* rook = dynamic_cast<const Rook*>(ptr)) {
            if (!rook->getMadeFirstMove()) {
                vmh.add(vmh.pos + Pos{direction*2, 0}, type);
            }
        }
    };
    auto checkCastlingLeft = [&] () {
        for (int i = vmh.pos.x()+1; i < 7; ++i) {
            if(vmh.b.at(i, vmh.pos.y()) != nullptr) return;
        }
        tryAddCastling(7, 1,  Move::Type::Castling);
    };
    auto checkCastlingRight = [&] () {
        for (int i = vmh.pos.x()-1; i >= 1; --i) {
            if(vmh.b.at(i, vmh.pos.y()) != nullptr) return;
        }
        tryAddCastling(0, -1,  Move::Type::QueensideCastling);
    };
    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            auto pos = vmh.pos + Pos(i, j);
            vmh.tryAdd(pos);
        }
    }

    if (!getMadeFirstMove()) {
        checkCastlingLeft();
        checkCastlingRight();
    }
}

void Knight::getValidMoves(ValidMovesHandler vmh) const {
    constexpr std::array pts = {
        Pos(1, 2),  Pos(2, 1),  Pos(1, -2),  Pos(-2, 1),
        Pos(-1, 2), Pos(2, -1), Pos(-1, -2), Pos(-2, -1),
    };
    for (auto p : pts)
        vmh.tryAdd(vmh.pos + p);
}

void Pawn::getValidMoves(ValidMovesHandler vmh) const {
    auto addCheckPromotion = [&](Pos pos, Move::Type t = Move::Type::Normal) {
        if (pos.y() == 0 || pos.y() == 7) {
            vmh.add(pos, Move::Type::Promotion);
        } else {
            vmh.add(pos, t);
        }
    };
    auto tryAddStraight = [&](Pos pos, Move::Type t = Move::Type::Normal) {
        if (!pos.isValid())
            return false;

        if (vmh.b.at(pos) != nullptr)
            return false;

        addCheckPromotion(pos, t);
        return true;
    };
    int sgn = getSide() == Side::White ? 1 : -1;
    auto myPos = vmh.pos;
    bool canMoveForward = tryAddStraight(myPos+Pos(0, sgn));

    if (canMoveForward && !getMadeFirstMove()) {
        tryAddStraight(myPos+Pos(0, 2*sgn), Move::Type::DoubleAdvance);
    }

    //check for eating
    for (int i : {-1, 1}) {
        auto pos = myPos + Pos(i, sgn);
        if (!pos.isValid())
            continue;

        const auto& ptr = vmh.b.at(pos);
        if (ptr != nullptr && ptr->getSide() != getSide())
            addCheckPromotion(pos);
    }

    //check for en passant
    for (int i : {-1, 1}) {
        auto pos = myPos + Pos(i, 0);
        if (!pos.isValid())
            continue;
        auto* ptr = vmh.b.at(pos);
        if (ptr == nullptr || ptr->getSide() == getSide())
        if (vmh.b.getEnPassantTarget() == pos) {
            //It can't be a promotion and a en passant at the same time
            vmh.add(myPos + Pos(i, sgn), Move::Type::EnPassant);
        }
    }
}

} //namespace chess
