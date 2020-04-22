#include "EndGameScene.h"

#include "MainScene.h"
#include "MainMenuScene.h"
#include "SoundManager.h"

using namespace core;

enum Button {
    ViewBoard,
    Rematch,
    MainMenu,
    Quit,

    BtnCount
};

constexpr int lerp(int a, int b, double t) {
    return int(a + t * (b-a));
}


EndGameScene::EndGameScene()
        : MenuScene({
            ButtonData::makeNormal("View Board"),
            ButtonData::makeSpriteRadio("Rematch"),
            ButtonData::makeSpriteRadio("Main Menu"),
            ButtonData::makeSpriteRadio("Quit"),
        }, Mode::Vertical), rects(3) {}

void EndGameScene::onCheckmate(chess::FullMove move, const std::string& msg) {
    instance().loadImpl(move, "Check Mate!", msg);
}

void EndGameScene::onStalemate(chess::FullMove move, const std::string& msg) {
    instance().loadImpl(move, "Stale Mate!", msg);
}
void EndGameScene::onGameDraw(chess::FullMove move, const std::string& msg) {
    instance().loadImpl(move, "Draw!", msg);
}

void EndGameScene::onStart() { MenuScene::onStart(); }
void EndGameScene::onMouseMove(core::Point pos) { MenuScene::onMouseMove(pos); }

const chess::Board& EndGameScene::getBoard() const {
    return MainScene::instance().getBoard();
}
const std::string& EndGameScene::getPlayerName(chess::Side s) const {
    return MainScene::instance().getPlayerName(s);
}
chess::Side EndGameScene::getPlayerSide() const {
    return MainScene::instance().getPlayerSide();
}

void EndGameScene::onSizeChanged(Point size) {
    constexpr Point titleSize {580, 130};
    constexpr Point subtitleSize {580, 80};

    BoardDrawingScene::onSizeChanged(size);
    auto sizes = {
        titleSize,
        subtitleSize,
        getVertButtonsSize(BtnCount),
    };
    auto spacing = {
        -25, 50
    };

    rects.updateCenteredVert(getBoardRect(), sizes, spacing, {10, 20});

    updateVertSizes(rects[2]);
}

constexpr const char* TitleFont = "Arial";
constexpr const char* SubtitleFont = "Arial";
constexpr int TitleSizeSmall = 16;
constexpr int TitleSizeBig = 110;
constexpr int SubtitleSize = 32;

void EndGameScene::finishAnimation() {
    state = State::FinishedAnimation;
    redrawBackground();
}

void EndGameScene::finishPieceMoving() {
    state = State::ShowingAnimation;
    t0 = std::chrono::system_clock::now();
    redrawBackground();
}
void EndGameScene::viewBoard() {
    state = State::ViewingBoard;
    redrawBackground();
}

void EndGameScene::onDrawBackground(Paint& p) {
    switch (state) {
    case State::ShowingPieceMoving:
        BoardDrawingScene::onDrawBackground(p);
        break;
    case State::ShowingAnimation:
        BoardDrawingScene::onDraw(p);
        return;
    case State::FinishedAnimation:
        p.fillRect(rects.boundingRect, MenuBgCol);
        p.drawRectOut(rects.boundingRect, MarginSize, MenuMarginCol);

        p.setTextColor(BigTextCol);
        p.setFont(TitleFont, TitleSizeBig, true);
        p.drawText(rects[0], title);

        p.setTextColor(MenuTextCol);
        p.setFont(SubtitleFont, SubtitleSize, true);
        p.drawText(rects[1], subtitle);

        MenuScene::onDrawBackground(p);
        break;
    case State::ViewingBoard:
        BoardDrawingScene::onDrawBackground(p);
        break;
    }
}

void EndGameScene::onDraw(Paint& p) {
    switch (state) {
    case State::ShowingPieceMoving:
        if (!pieceMovingData.isMoving()) {
            finishPieceMoving();
            redraw();
        } else {
            BoardDrawingScene::onDraw(p);
        }
        break;
    case State::ShowingAnimation: {
        using namespace std;
        constexpr double waitMs = 200;
        constexpr double durationMs = 1000;

        auto now = chrono::system_clock::now();
        auto dt = chrono::duration_cast<chrono::milliseconds>(now - t0).count();
        if (dt >= waitMs + durationMs) {
            finishAnimation();
            return;
        } else if (dt > waitMs) {
            int size = lerp(TitleSizeSmall, TitleSizeBig,
                            (dt-waitMs)/durationMs);
            p.setFont(TitleFont,size, true);

            p.setTextColor(BigTextCol);
            p.drawText(rects[0], title);
        }
        redraw();
    } break;
    case State::FinishedAnimation:
        MenuScene::onDraw(p);
        break;
    case State::ViewingBoard:
        BoardDrawingScene::onDraw(p);
        break;
    }
}

bool EndGameScene::checkUserInput() {
    switch (state) {
    case State::ShowingPieceMoving:
        break;
    case State::ShowingAnimation:
        finishAnimation();
        break;
    case State::FinishedAnimation:
        return true;
    case State::ViewingBoard:
        state = State::FinishedAnimation;
        redrawBackground();
        break;
    }

    return false;
}
void EndGameScene::onLeftMouseDown(Point pos) {
    if (!checkUserInput()) return;
    MenuScene::onLeftMouseDown(pos);
}
void EndGameScene::onKeyDown(char k) {
    if (!checkUserInput()) return;
    MenuScene::onKeyDown(k);
}

void EndGameScene::loadImpl(chess::FullMove move,
                            std::string_view title,
                            const std::string& subtitle) {
    this->title = title;
    this->subtitle = subtitle;
    SceneManager::load(*this);
    if (!move.to.isValid() || !move.from.isValid()) {
        pieceMovingData.stop();
    } else {
        pieceMovingData.setInterpolatedMove(move);
    }
    state = State::ShowingPieceMoving;
    redrawBackground();
}

void EndGameScene::onButtonSelected(int i) {
    SoundManager::playSelected();
    switch (i) {
    case Button::MainMenu:
        SceneManager::load<MainMenuScene>();
        break;
    case Button::ViewBoard:
        viewBoard();
        break;
    case Button::Quit:
        quit();
        break;
    case Button::Rematch:
        MainScene::newGame(MainScene::instance().getPlayerSide());
        break;
    }
}
