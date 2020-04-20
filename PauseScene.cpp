#include "PauseScene.h"
#include "MainScene.h"
#include "EndGameScene.h"
#include "MainMenuScene.h"
#include "OptionsScene.h"

using namespace core;

enum Button : int {
    Continue = 0,
    OfferDraw,
    MainMenu,
    Options,
    Quit,

    BtnCount,
};

void PauseScene::onReturnFromOptions() {
    WindowHandler::instance().setSnapshot(instance().snapshot);
    SceneManager::load(instance());
}

PauseScene::PauseScene()
        : MenuScene({
            ButtonData::makeNormal("Continue"),
            ButtonData::makeNormal("Offer Draw"),
            ButtonData::makeNormal("Main Menu"),
            ButtonData::makeNormal("Options"),
            ButtonData::makeNormal("Quit"),
        }, Mode::Vertical), rects(2), useSnapshot(false) {}

void PauseScene::onSizeChanged(core::Point size) {
    constexpr Point titleSize {580, 130};

    constexpr int spacing = 10;
    rects.updateCenteredVert(MainScene::getBoardRect(size), titleSize,
                             getVertButtonsSize(BtnCount), spacing,
                             {10, 20});
    updateVertSizes(rects[1]);
}

void PauseScene::onDrawBackground(core::Paint& p) {
    constexpr const char* titleFont = "Arial";

    p.fill(Color::Blurry);
    p.fillRect(rects.boundingRect, MenuBgCol);
    p.drawRectOut(rects.boundingRect, MarginSize, MenuMarginCol);
    p.setTextColor(BigTextCol);
    p.setFont(titleFont, SmallTitleSize, true);
    p.drawText(rects[0], "Pause");

    MenuScene::onDrawBackground(p);
}

void PauseScene::onButtonSelected(int index) {
    MenuScene::onButtonSelected(index);
    switch (index) {
    case Button::Continue:
        SceneManager::load<MainScene>();
        break;
    case Button::MainMenu:
        SceneManager::load<MainMenuScene>();
        break;
    case Button::Options:
        WindowHandler::instance().getSnapshot(snapshot);
        OptionsScene::load(onReturnFromOptions);
        break;
    case Button::OfferDraw:
        SceneManager::load<MainScene>();
        EndGameScene::onGameDraw({}, "Both players agreed to the draw.");
        break;
    case Button::Quit:
        quit();
        break;
    }
}

