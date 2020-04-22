#pragma once
#include "core/ButtonSelectorScene.h"
#include "chess/Piece.h"

class PromotionScene : public core::ButtonSelectorScene {
public:
    static PromotionScene& instance() {
        static PromotionScene s;
        return s;
    }

    static void onPromotion(chess::Side side);

    void onSizeChanged(core::Point size) override;

    void onDraw(core::Paint& p) override;
    void onDrawBackground(core::Paint& p) override;

    void onButtonSelected(int i) override;

    void drawMouse(core::Paint& p, const core::Rect& r) override;
    void drawKeyboard(core::Paint& p, const core::Rect& r) override;

private:
    PromotionScene();

    chess::Side side = chess::Side::White;
    core::RectGroup rects;
};
