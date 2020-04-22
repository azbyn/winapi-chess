#pragma once

#include "core/Scene.h"
#include "core/RectGroup.h"

constexpr int SquareLength = 64;
constexpr core::Point SquareSize = core::Point(1,1) * SquareLength;
constexpr int MarginSize = 3;

constexpr core::Color WhiteSquareCol = 0xc3ac8c;
constexpr core::Color BlackSquareCol = 0x3a1d17;

constexpr core::Color MarginCol = core::Color::grayscale(16);

constexpr core::Color ButtonCol = BlackSquareCol;
constexpr core::Color ButtonTextCol = core::Color::White;
constexpr core::Color ButtonHighlightCol = WhiteSquareCol.withAlpha(50);
constexpr core::Color MenuBgCol = WhiteSquareCol;
constexpr core::Color MenuTextCol = core::Color::Black;
constexpr core::Color MenuMarginCol = BlackSquareCol;
constexpr core::Color BigTextCol = core::Color::Red;


constexpr core::Color SelectedColor = core::Color::Green.withAlpha(200);
constexpr core::Color ValidColor    = core::Color::Blue.withAlpha(200);

constexpr core::Point VertButtonSize {350, 64};
constexpr int ButtonSpacing = 5;
constexpr int SmallTitleSize = 80;

constexpr core::Point getVertButtonsSize(int count) {
    return {VertButtonSize.x,
            (VertButtonSize.y + ButtonSpacing) * count - ButtonSpacing};
}

struct SceneManager {
    SceneManager() = delete;

    inline static void load(core::Scene& s) {
        core::WindowHandler::instance().changeScene(s);
    }
    template<typename T>
    inline static void load() {
        static_assert(std::is_base_of_v<core::Scene, T>, "T must be a Scene");
        load(T::instance());
    }
};

