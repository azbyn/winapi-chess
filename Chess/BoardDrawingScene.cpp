#include "BoardDrawingScene.h"

using namespace core;

constexpr int BoardBorder = SquareLength / 2;
constexpr int RightPaneMargin = BoardBorder * 3 / 4;
constexpr auto RightPaneWidth = SquareLength * 4;

BoardDrawingScene::BoardDrawingScene()
        : pieceMovingData(*this) {}

void BoardDrawingScene::onSizeChanged(Point size) {
    static Point lastBadSize {-1, -1};
    boardRect = getBoardRect(size);
    if (boardRect.left < 0 || boardRect.top < 0) {
        if (lastBadSize != size) {
            warning("Window size too small");
            lastBadSize = size;
        }
    }

    int paneStartX = boardRect.right + BoardBorder + RightPaneMargin;
    paneRect = {
        paneStartX,
        boardRect.top - BoardBorder,
        paneStartX + RightPaneWidth,
        boardRect.bottom + BoardBorder
    };
}

core::Point BoardDrawingScene::boardPosToScreen(chess::Pos boardPos) const {
    if (getPlayerSide() == chess::Side::White)
        boardPos.y() = 7 - boardPos.y();
    return boardStart() + boardPos * SquareLength;
}
chess::Pos BoardDrawingScene::screenToBoardPos(core::Point pt) const {
    auto pos = (pt - boardStart()) / SquareLength;
    if (getPlayerSide() == chess::Side::White)
        pos.y = 7 - pos.y;

    pos.x = core::clamp(pos.x, 0, 7);
    pos.y = core::clamp(pos.y, 0, 7);
    return chess::Pos(pos);
}

void BoardDrawingScene::drawBoardSquares(Paint& p) const {
    for (int j = 0; j < 8; ++j) {
        for (int i = 0; i < 8; ++i) {
            auto pt = boardPosToScreen({i, j});
            auto col = (i ^ j) % 2 == 1 ? WhiteSquareCol : BlackSquareCol;
            p.fillRect({pt, pt + SquareSize}, col);
        }
    }
}

void BoardDrawingScene::drawBoardMarkings(Paint& p) const {
    p.setFont("Arial", 32, false);
    p.setTextColor(BlackSquareCol);
    char str[2] = {};
    for (int i = 0; i < 8; ++i) {
        str[0] = 'A' + i;
        Point top = boardStart() + Point(i * SquareLength, 0);
        Point bot = boardStart() + Point(i * SquareLength, 8 * SquareLength);
        p.drawText({top, top + Point(SquareLength, -BoardBorder)}, str);

        p.drawText({bot, bot + Point(SquareLength, +BoardBorder)}, str);
    }

    for (int i = 0; i < 8; ++i) {
        str[0] = getPlayerSide() == chess::Side::White ? ('8' - i) : ('1' + i);
        Point left = boardStart() + Point(0, i * SquareLength);
        p.drawText({left, left + Point(-BoardBorder, SquareLength)}, str);

        Point right = boardStart() + Point(8 * SquareLength, i * SquareLength);
        p.drawText({right, right + Point(BoardBorder, SquareLength)}, str);
    }
}

void BoardDrawingScene::onDrawBackground(Paint& p) {
    p.fill(MarginCol);
    p.drawRectOut(boardRect, BoardBorder, MenuBgCol);

    p.drawRectOut(boardRect, MarginSize, MenuMarginCol);

    p.fillRect(paneRect, MenuBgCol);
    p.drawRectIn(paneRect, MarginSize, MenuMarginCol);

    drawBoardSquares(p);
    drawBoardMarkings(p);
}
void BoardDrawingScene::drawRightPaneInfo(Paint& p) const {
    p.setTextColor(MenuTextCol);
    int textHeight = 32;
    int offset = MarginSize;
    Rect rect1 {
        paneRect.left + MarginSize,
        paneRect.top + offset,
        paneRect.right - MarginSize,
        paneRect.top + textHeight + offset,
    };
    Rect rect2 {
        paneRect.left + MarginSize,
        paneRect.bottom - textHeight - offset,
        paneRect.right - MarginSize,
        paneRect.bottom - offset,
    };
    auto topSide = getOtherSide(getPlayerSide());
    auto botSide = getPlayerSide();
    auto& currentRect = getBoard().getCurrentSide() == topSide ? rect1 : rect2;
    p.fillRect(currentRect, SelectedColor);

    p.drawText(rect1, getPlayerName(topSide));
    p.drawText(rect2, getPlayerName(botSide));

    p.setTextColor(Color::Red);
    if (getBoard().getIsInCheck(topSide)) {
        p.drawText(rect1 + Point(0, rect1.height()), "CHECK");
    } else if (getBoard().getIsInCheck(botSide)) {
        p.drawText(rect2 - Point(0, rect2.height()), "CHECK");
    }
}
void BoardDrawingScene::drawEatenPieces(Paint& p) const {
    // making the sprites slightly overlap looks good
    constexpr int pieceSize = 30;
    constexpr int fullPieceSize = 32;
    constexpr int lineSpacing = 28;
    constexpr int startY = 40;

    auto drawPieces = [&] (Point start, auto& pieces) {
        auto it = pieces.begin();
        auto end = pieces.end();
        for (int i = 0; i < 8; ++i) {
            if (it >= end) return;
            auto& piece = *(*it++);
            Point pos = start + Point{i * pieceSize, 0};
            p.drawSprite(pos, piece.getSprite(), piece.getPalette());
        }

        start += Point(pieceSize/2, lineSpacing);
        int i = 0;
        for (; it < end ; ++it, ++i) {
            Point pos = start + Point{i * pieceSize, 0};
            auto& piece = *(*it);
            p.drawSprite(pos, piece.getSprite(), piece.getPalette());
        }
    };
    int startX = (paneRect.width() - pieceSize * 8 -
                  (fullPieceSize - pieceSize)) / 2;

    Point topPos = paneRect.topLeft() + Point(startX, startY);
    Point botPos{paneRect.left + startX,
                 paneRect.bottom - startY - fullPieceSize - pieceSize};

    auto topSide = getOtherSide(getPlayerSide());
    auto botSide = getPlayerSide();

    drawPieces(topPos, getBoard().getEatenPieces(topSide));
    drawPieces(botPos, getBoard().getEatenPieces(botSide));
}

void BoardDrawingScene::onDraw(Paint& paint) {
    drawBoard(paint);
    drawEatenPieces(paint);
    drawRightPaneInfo(paint);

    if (pieceMovingData.isMoving()) {
        auto& piece = getBoard().at(pieceMovingData.getPiecePos());
        auto pt = pieceMovingData.getPoint();

        spriteAt(paint, pt, piece->getSprite(), piece->getPalette());
        redraw();
    }
}

void BoardDrawingScene::drawBoard(Paint& p) const {
    for (int j = 0; j < 8; ++j) {
        for (int i = 0; i < 8; ++i) {
            chess::Pos pos {i, j};
            auto& ptr = getBoard().at(pos);

            if (ptr && pos != pieceMovingData.getPiecePos()) {
                spriteOnBoard(p, pos, ptr->getSprite(), ptr->getPalette());
            }
        }
    }
}

void BoardDrawingScene::PieceMovingData::setInterpolatedMove(
        chess::FullMove move) {
    interp.from = parent.boardPosToScreen(move.from);
    interp.to = parent.boardPosToScreen(move.to);
    state = State::Interpolating;

    piecePos = move.to; // this is where the piece is now

    t0 = std::chrono::system_clock::now();
}

Point BoardDrawingScene::PieceMovingData::getPoint() {
    using namespace std;
    switch (state) {
    case State::Interpolating: {
        constexpr double durationMs = 250;
        auto now = chrono::system_clock::now();
        auto dt = chrono::duration_cast<chrono::milliseconds>(now - t0).count();
        if (dt >= durationMs) {
            state = State::NotMoving;
            piecePos = chess::Pos::Invalid;
            return interp.to;
        }
        return Point::lerp(interp.from, interp.to, dt / durationMs);
    }
    case State::MouseMoving:
        return mouse.current - mouse.offset;
    default:
        warning("getPoint(): invalid state");
        return {};
    }

}

void BoardDrawingScene::PieceMovingData::setMousePos(
        Point p, chess::Pos boardPos) {
    state = State::MouseMoving;
    piecePos = boardPos;
    mouse.offset = p;
}
bool BoardDrawingScene::PieceMovingData::onMouseMove(Point p) {
    auto val = state == State::MouseMoving;
    if (val) {
        mouse.current = p;
    }
    return val;
}
void BoardDrawingScene::PieceMovingData::onLeftMouseUp() {
    if (state == State::MouseMoving) {
        state = State::NotMoving;
        piecePos = chess::Pos::Invalid;
    }
}
void BoardDrawingScene::PieceMovingData::stop() {
    state = State::NotMoving;
    piecePos = chess::Pos::Invalid;
}
void BoardDrawingScene::spriteAt(core::Paint& paint, core::Point pt,
                                 const core::PaletteSprite& s,
                                 const core::Palette& palette) const {
    size_t scale = SquareLength / s.width();
    paint.drawSprite(pt, s, palette, scale);
}
void BoardDrawingScene::spriteOnBoard(core::Paint& paint, chess::Pos p,
                                      const core::PaletteSprite& s,
                                      const core::Palette& palette) const {
    spriteAt(paint, boardPosToScreen(p), s, palette);
}


Rect BoardDrawingScene::getBoardRect(Point size) {
    Point start {
        (size.x - SquareLength*8 - RightPaneWidth - RightPaneMargin)/2,
        (size.y - SquareLength*8)/2,
    };
    return {start, start + SquareSize*8};
}
