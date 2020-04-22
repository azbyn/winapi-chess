#pragma once

#include "BoardDrawingScene.h"
#include "chess/Board.h"
#include "chess/Bot.h"

#include <chrono>

class MainScene : public BoardDrawingScene {
public:
    static MainScene& instance() {
        static MainScene s;
        return s;
    }

    static void newGame(chess::Side side) {
        instance().newGameImpl(side);
    }

    void drawBoard(core::Paint& paint) const override;

    void onKeyDown(char k) override;

    void onLeftMouseUp(core::Point pos) override;
    void onLeftMouseDown(core::Point pos) override;
    void onMouseMove(core::Point pos) override;

    void updateBotDifficulty();

    static bool getShowingValidMoves();
    static void setShowingValidMoves(bool val);
    static void toggleShowingValidMoves();

    chess::Board& getBoard() { return board; }

    const chess::Board& getBoard() const override { return board; }
    chess::Side getPlayerSide() const override { return playerSide; }
    const std::string& getPlayerName(chess::Side s) const override {
        return playerNames[s];
    }

private:
    MainScene();
    void newGameImpl(chess::Side side);

    chess::Side playerSide;
    chess::SideEntries<std::string> playerNames;

    chess::Board board;
    chess::Bot bot;

    chess::Pos selectedPos;
    chess::Pos cursor;
    std::vector<chess::Move> validMoves;

    bool showingValidMoves;

    bool isSelected() const;
    void deselect();

    // Fails if there's no piece at pos or
    // if the piece is not the correct side
    bool trySelect(chess::Pos pos);

    void moveVert(int dir);
    void spaceAction();

    static void onExecutedMove(chess::FullMove m);
    static void onFoundMove(chess::FullMove m);
};
