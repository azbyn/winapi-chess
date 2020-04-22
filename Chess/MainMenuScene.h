#pragma once

#include "MenuScene.h"
#include "chess/Common.h"

class MainMenuScene : public MenuScene {
public:
    static MainMenuScene& instance() {
        static MainMenuScene s;
        return s;
    }

    void onSizeChanged(core::Point size) override;

    void onDrawExtra(core::Paint& p) override;
    void onDrawBackground(core::Paint& p) override;

    void onButtonSelected(int i) override;

private:
    MainMenuScene();

    core::RectGroup rects;

    chess::Side side;
    bool stretch;
    bool isFullScreen;
};

