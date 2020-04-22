#pragma once

#include "SceneCommon.h"
#include "chess/Board.h"

#include <chrono>

class BoardDrawingScene : public virtual core::Scene {
public:
    static core::Rect getBoardRect(core::Point size);

    // Must be called by subclass
    void onSizeChanged(core::Point size) override;
    // Must be called by subclass
    void onDrawBackground(core::Paint& paint) override;
    // Must be called by subclass
    void onDraw(core::Paint& paint) override;

protected:
    virtual const chess::Board& getBoard() const = 0;
    virtual const std::string& getPlayerName(chess::Side) const = 0;
    virtual chess::Side getPlayerSide() const = 0;

    BoardDrawingScene();

    core::Point boardPosToScreen(chess::Pos boardPos) const;
    chess::Pos screenToBoardPos(core::Point pt) const;

    virtual void drawBoardSquares(core::Paint& paint) const;
    // A..H, 1..8
    virtual void drawBoardMarkings(core::Paint& paint) const;
    virtual void drawEatenPieces(core::Paint& paint) const;
    //must be called
    virtual void drawBoard(core::Paint& paint) const;
    virtual void drawRightPaneInfo(core::Paint& paint) const;

    void spriteAt(core::Paint& paint, core::Point pt,
                  const core::PaletteSprite& s,
                  const core::Palette& palette) const;

    void spriteOnBoard(core::Paint& paint, chess::Pos p,
                       const core::PaletteSprite& s,
                       const core::Palette& palette) const;

    const core::Rect& getBoardRect() const { return boardRect; }

private:
    // without the markings (the part with 1..8, A..H)
    core::Rect boardRect;
    core::Rect paneRect;

protected:
    // Move interpolating
    struct PieceMovingData {
        enum class State {
            NotMoving,
            Interpolating,
            MouseMoving,
        };

        PieceMovingData(const BoardDrawingScene& parent) : parent(parent) {}
        void reset() {
            state = State::NotMoving;
            piecePos = chess::Pos::Invalid;
        }
        constexpr bool isMoving() const { return state != State::NotMoving; }
        constexpr chess::Pos getPiecePos() const { return piecePos; }

        core::Point getPoint();

        void setInterpolatedMove(chess::FullMove move);

        void setMousePos(core::Point p, chess::Pos boardPos);
        // Returns true if should redraw
        bool onMouseMove(core::Point p);
        void onLeftMouseUp();

        void stop();

    private:
        const BoardDrawingScene& parent;
        chess::Pos piecePos = chess::Pos::Invalid;
        State state = State::NotMoving;

        union {
            struct {
                core::Point from, to;
            } interp;
            struct {
                core::Point offset, current;
            } mouse;
        };

        std::chrono::time_point<std::chrono::system_clock> t0;
    } pieceMovingData;

private:
    core::Point boardStart() const { return boardRect.topLeft(); }
};
