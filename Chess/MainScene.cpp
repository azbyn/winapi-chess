#include "MainScene.h"

#include "PromotionScene.h"
#include "EndGameScene.h"
#include "PauseScene.h"
#include "SoundManager.h"

using namespace core;

static void onPromotion(chess::Side side) {
    PromotionScene::onPromotion(side);
}
static void onCheckmate(chess::FullMove move, chess::Side whoWon) {
    EndGameScene::onCheckmate(move,
        concat(whoWon, " (", MainScene::instance().getPlayerName(whoWon),
               ") won!"));
}

static void onStalemate(chess::FullMove move, chess::Side whoCantMove) {
    EndGameScene::onStalemate(move, concat(whoCantMove, " can't move"));
}
static void onGameDraw(chess::FullMove move, std::string_view why) {
    EndGameScene::onGameDraw(move, std::string(why));
}
void MainScene::onExecutedMove(chess::FullMove) {
    instance().bot.onPlayerMove(instance().board.getState());
}

void MainScene::onFoundMove(chess::FullMove m) {
    auto& i = instance();
    i.board.doFullMove(m);
    i.pieceMovingData.setInterpolatedMove(m);
    i.redraw();
    SoundManager::playPieceMove();
}

MainScene::MainScene()
        : board(onPromotion, onCheckmate, onStalemate, onGameDraw),
          bot(onFoundMove) {
    showingValidMoves = true;
}

bool MainScene::getShowingValidMoves() {
    return instance().showingValidMoves;
}
void MainScene::setShowingValidMoves(bool val) {
    instance().showingValidMoves = val;
}
void MainScene::toggleShowingValidMoves() {
    instance().showingValidMoves = !instance().showingValidMoves;
}

void MainScene::updateBotDifficulty() {
    playerNames[getOtherSide(playerSide)] = concat("Bot ", bot.getDifficulty());
}
void MainScene::newGameImpl(chess::Side side) {
    playerSide = side;
    cursor = {4, side == chess::Side::White ? 0 : 7}; //the king

    playerNames[playerSide] = "Player";
    playerNames[getOtherSide(playerSide)] = concat("Bot ", bot.getDifficulty());
    selectedPos = chess::Pos::Invalid;
    pieceMovingData.reset();
    board.reset();
    bot.reset();
    validMoves.clear();
    SceneManager::load(*this);
    if (side != chess::Side::White)
        bot.onPlayerMove(board.getState());
}

void MainScene::drawBoard(Paint& paint) const {
    if (isSelected()) {
        auto pt = boardPosToScreen(selectedPos);
        paint.fillRect({pt, pt + SquareSize}, SelectedColor);
    }
    BoardDrawingScene::drawBoard(paint);
    if (showingValidMoves) {
        for (auto p : validMoves) {
            auto pt = boardPosToScreen(p.pos) + SquareSize / 2;
            paint.fillPixelatedCircle(pt, SquareLength / 4, ValidColor, 2);
        }
    }
    spriteOnBoard(paint, cursor, sprites::Cursor, sprites::CursorPalette);
}
void MainScene::moveVert(int dir) {
    if (playerSide != chess::Side::White) dir *= -1;
    cursor.y() += dir;
    if (cursor.y() < 0) cursor.y() = 0;
    if (cursor.y() > 7) cursor.y() = 7;
}

bool MainScene::isSelected() const { return selectedPos.isValid(); }
void MainScene::deselect() {
    validMoves.clear();
    selectedPos = chess::Pos::Invalid;
    redraw();
}
bool MainScene::trySelect(chess::Pos pos) {
    deselect();
    auto& val = board.at(pos);

    if (!val || val->getSide() != playerSide)
        return false;
    val->getValidMoves(pos, board.getState(), validMoves);

    selectedPos = pos;
    return true;
}

void MainScene::spaceAction() {
    if (board.getCurrentSide() != playerSide) {
        deselect();
        return;
    }

    if (isSelected()) {
        if (selectedPos == cursor ||
            board.tryMove(selectedPos, cursor, onExecutedMove)) {

            deselect();
            return;
        }
    }
    trySelect(cursor);
}
void MainScene::onLeftMouseDown(Point pt) {
    if (getBoardRect().contains(pt)) {
        auto pos = screenToBoardPos(pt);
        cursor = pos;
        if (isSelected()) {
            if (selectedPos == cursor ||
                board.tryMove(selectedPos, cursor, onExecutedMove)) {

                deselect();
                return;
            }
        }
        if (!trySelect(pos))
            return;

        auto offset = pt - boardPosToScreen(pos);
        pieceMovingData.setMousePos(offset, pos);
        pieceMovingData.onMouseMove(pt);

        redraw();
    }
}
void MainScene::onLeftMouseUp(Point pt) {
    if (pieceMovingData.onMouseMove(pt)) {
        cursor = screenToBoardPos(pt);

        auto pos = screenToBoardPos(pt);
        cursor = pos;

        if (selectedPos != cursor) {
            board.tryMove(selectedPos, cursor, onExecutedMove);
            deselect();
        }

        pieceMovingData.onLeftMouseUp();
        redraw();
    }
}
void MainScene::onMouseMove(Point pt) {
    if (pieceMovingData.onMouseMove(pt)) {
        cursor = screenToBoardPos(pt);
        redraw();
    }
}

void MainScene::onKeyDown(char k) {
    switch (k) {
    case VK_ESCAPE:
    case 'Q':
    case 'P':
        SceneManager::load<PauseScene>();
        break;
    case 'A':
    case VK_LEFT:
        if (cursor.x() > 0) --cursor.x();
        break;
    case 'D':
    case VK_RIGHT:
        if (cursor.x() < 7) ++cursor.x();
        break;
    case 'W':
    case VK_UP:
        moveVert(1);
        break;
    case 'S':
    case VK_DOWN:
        moveVert(-1);
        break;
    case VK_RETURN:
    case VK_SPACE:
        spaceAction();
        break;
    default:
        return;
    }
    redraw();
}
