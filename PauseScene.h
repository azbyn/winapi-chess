#pragma once

#include "MenuScene.h"

class PauseScene : public MenuScene {
public:
    static PauseScene& instance() {
        static PauseScene s;
        return s;
    }

    void onSizeChanged(core::Point size) override;

    void onDrawBackground(core::Paint& p) override;

    void onButtonSelected(int i) override;

private:
    PauseScene();

    core::RectGroup rects;
    bool useSnapshot;
    std::vector<uint8_t> snapshot;
    static void onReturnFromOptions();
};

