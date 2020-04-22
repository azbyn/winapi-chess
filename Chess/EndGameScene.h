#pragma once

#include "MenuScene.h"
#include "BoardDrawingScene.h"
#include "chess/Common.h"

#include <chrono>

// Could be done without having 'The Diamond Problem'
// but where's the fun in that?
class EndGameScene : public virtual MenuScene,
                     public virtual BoardDrawingScene {
public:
    static EndGameScene& instance() {
        static EndGameScene s;
        return s;
    }

    static void onCheckmate(chess::FullMove move, const std::string& msg);
    static void onStalemate(chess::FullMove move, const std::string& msg);
    static void onGameDraw (chess::FullMove move, const std::string& msg);

    void onSizeChanged(core::Point size) override;

    void onStart() override;

    void onDraw(core::Paint& p) override;
    void onDrawBackground(core::Paint& p) override;

    void onKeyDown(char k) override;
    void onLeftMouseDown(core::Point pos) override;

    void onButtonSelected(int i) override;
    void onMouseMove(core::Point pos) override;

    // BoardDrawingScene
    const chess::Board& getBoard() const override;
    const std::string& getPlayerName(chess::Side) const override;
    chess::Side getPlayerSide() const override;

private:
    enum class State {
        ShowingPieceMoving,
        ShowingAnimation,
        FinishedAnimation,
        ViewingBoard,
    };
    EndGameScene();

    std::string title, subtitle;
    std::chrono::time_point<std::chrono::system_clock> t0;
    State state;

    core::RectGroup rects;

    State returnFromBoardViewing();
    void loadImpl(chess::FullMove move,
                  std::string_view title,
                  const std::string& subtitle);

    // Called from onLeftMouseDown and onKeyDown
    // if state == ViewingBoard then we resume
    // returns true if we should pass the input to MenuScene
    bool checkUserInput();
    void finishPieceMoving();
    void finishAnimation();
    void viewBoard();
};

