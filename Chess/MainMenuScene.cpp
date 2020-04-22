#include "MainMenuScene.h"

#include "SceneCommon.h"
#include "OptionsScene.h"
#include "MainScene.h"
#include "Sprites.h"
#include "chess/Piece.h"
#include "chess/Bot.h"

#include <cmath>
#include <algorithm>

using namespace core;

enum Button : int {
    Play = 0,
    Side,
    Difficulty,
    Options,
    Quit,

    BtnCount,
};

static void onReturnFromOptions() {
    SceneManager::load<MainMenuScene>();
}

MainMenuScene::MainMenuScene()
        : MenuScene({
            ButtonData::makeNormal("Play"),
            ButtonData::makeSpriteRadio("Side"),
            ButtonData::makeLeftRight("Difficulty",
                                      chess::Bot::MinDifficulty,
                                      chess::Bot::MaxDifficulty,
                                      chess::Bot::getDifficulty,
                                      chess::Bot::setDifficulty),
            ButtonData::makeNormal("Options"),
            ButtonData::makeNormal("Quit"),
       }, Mode::Vertical), rects(2) {}

void MainMenuScene::onSizeChanged(core::Point size) {
    constexpr Point titleSize {580, 130};

    constexpr int spacing = 10;
    rects.updateCenteredVert(size.asRect(), titleSize,
                             getVertButtonsSize(BtnCount), spacing,
                             {10, 20});
    updateVertSizes(rects[1]);
}

void MainMenuScene::onDrawBackground(core::Paint& p) {
    constexpr const char* titleFont = "Arial";
    constexpr int titleSize = 110;

    p.fill(MenuBgCol);
    p.setTextColor(BigTextCol);
    p.setFont(titleFont, titleSize, true);
    p.drawText(rects[0], "Chess");

    MenuScene::onDrawBackground(p);
}
void MainMenuScene::onDrawExtra(Paint& p) {
    // side
    auto rect = getButtonRect(Button::Side);

    auto& sprite = sprites::King;
    auto pt = rect.p1() - (rect.height() * Point{1,1} - sprite.size() / 2);
    p.drawSprite(pt, sprite, chess::Piece::getPalette(side));
}
void MainMenuScene::onButtonSelected(int index) {
    MenuScene::onButtonSelected(index);
    switch (index) {
    case Button::Play:
        MainScene::newGame(side);
        return;
    case Button::Side:
        side = getOtherSide(side);
        break;
    case Button::Difficulty:
        break;
    case Button::Options:
        OptionsScene::load(onReturnFromOptions);
        break;
    case Button::Quit:
        quit();
        return;
    }
    redraw();
}

