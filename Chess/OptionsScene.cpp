#include "OptionsScene.h"
#include "MainScene.h"
#include "MainMenuScene.h"
#include "SoundManager.h"

using namespace core;

enum Button : int {
    Back = 0,
    SfxOn,
    SfxVolume,
    Difficulty,
    ShowValidMoves,
    IsResizeable,

    BtnCount,
};

static bool getIsResizeable() {
    return WindowHandler::instance().getWindowMode() != WindowMode::Static;
}

OptionsScene::OptionsScene()
        : MenuScene({
            ButtonData::makeNormal("Back"),
            ButtonData::makeRadio("Sfx",
                                  SoundManager::getSfxOn),
            ButtonData::makeLeftRight("Sfx Volume",
                                      SoundManager::MinNiceVolume,
                                      SoundManager::MaxNiceVolume,
                                      SoundManager::getNiceVolume,
                                      SoundManager::setNiceVolume),
            ButtonData::makeLeftRight("Difficulty",
                                      chess::Bot::MinDifficulty,
                                      chess::Bot::MaxDifficulty,
                                      chess::Bot::getDifficulty,
                                      chess::Bot::setDifficulty),
            ButtonData::makeRadio("Show valid moves",
                                  MainScene::getShowingValidMoves),
            ButtonData::makeRadio("Is Resizeable", getIsResizeable),
        }, Mode::Vertical), rects(2) {}


void OptionsScene::load(Callback onBack) {
    instance().onBack = onBack;
    SceneManager::load(instance());
}

void OptionsScene::onSizeChanged(core::Point size) {
    constexpr Point titleSize {580, 130};

    constexpr int spacing = 10;
    rects.updateCenteredVert(size.asRect(), titleSize,
                             getVertButtonsSize(BtnCount), spacing,
                             {10, 20});
    updateVertSizes(rects[1]);
}

void OptionsScene::onDrawBackground(core::Paint& p) {
    constexpr const char* titleFont = "Arial";

    p.fill(MenuBgCol);
    p.setTextColor(BigTextCol);
    p.setFont(titleFont, SmallTitleSize, true);
    p.drawText(rects[0], "Options");

    MenuScene::onDrawBackground(p);
}

void OptionsScene::onButtonSelected(int index) {
    MenuScene::onButtonSelected(index);
    switch (index) {
    case Button::Back:
        MainScene::instance().updateBotDifficulty();
        onBack();
        break;
    case Button::Difficulty:
    case Button::SfxVolume:
        break;
    case Button::SfxOn:
        SoundManager::toggleSfxOn();
        redraw();
        break;
    case Button::ShowValidMoves:
        MainScene::toggleShowingValidMoves();
        redraw();
        break;
    case Button::IsResizeable: {
        auto& wh = WindowHandler::instance();
        if (wh.getWindowMode() == WindowMode::Static) {
            wh.setWindowMode(WindowMode::Resizeable);
        } else {
            wh.setWindowMode(WindowMode::Static);
        }
    } break;
    }
}
