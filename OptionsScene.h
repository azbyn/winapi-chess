#pragma once

#include "MenuScene.h"

class OptionsScene : public MenuScene {
public:
    using Callback = void(*)();
    static OptionsScene& instance() {
        static OptionsScene s;
        return s;
    }

    static void load(Callback onBack);

    void onSizeChanged(core::Point size) override;
    void onDrawBackground(core::Paint& p) override;
    void onButtonSelected(int i) override;

private:
    OptionsScene();
    Callback onBack;

    core::RectGroup rects;
};
