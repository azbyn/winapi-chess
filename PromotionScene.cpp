#include "PromotionScene.h"

#include "chess/Board.h"
#include "MainScene.h"
#include "SoundManager.h"
#include "SceneCommon.h"

using namespace core;

constexpr int SquareSpacing = 12;

void PromotionScene::onPromotion(chess::Side side) {
    auto& i = instance();
    i.side = side;
    SceneManager::load<PromotionScene>();
}

PromotionScene::PromotionScene()
        : ButtonSelectorScene(4, Mode::Horizontal), rects(2) {}

void PromotionScene::onSizeChanged(Point size) {
    std::clog << "PromotionScene.onSizeChanged("<< size<<")\n";
    constexpr int topTextHeight = 80;
    constexpr int margins = 32;
    constexpr int spacing = 32;
    constexpr int textWidth = 80 + 4 * SquareLength;
    constexpr int squaresWidth = 4 * SquareLength + 3 * SquareSpacing;
    rects.updateCenteredVert(MainScene::getBoardRect(size),
                             {textWidth, topTextHeight},
                             {squaresWidth, SquareLength},
                             spacing, margins);
    getButtonRects().updateEquallySpacedHori(rects[1], SquareSize);
}

constexpr int PieceMargins = 2;
void PromotionScene::onDrawBackground(Paint& p) {
    p.setFont("Arial", 32, false);
    std::clog << "PromotionScene.onDrawBackground()\n";
    p.fill(Color::Blurry);
    p.fillRect(rects.boundingRect, MenuBgCol);
    p.drawRectOut(rects.boundingRect, MarginSize, MenuMarginCol);
    p.setTextColor(MenuTextCol);
    p.drawText(rects[0], "Please select a piece to\n promote to:",
               textFormat::MultilineCenter);
    // using namespace sprites;
    for (int i = 0; i < 4; ++i) {
        auto pos = getButtonRect(i).p0();
        Rect rect = {pos, pos+SquareSize};
        p.fillRect(rect, BlackSquareCol);
        p.drawRectOut(rect, PieceMargins, MarginCol);
    }
}

void PromotionScene::drawMouse(Paint& p, const Rect& rect) {
    p.fillRect(rect, ButtonHighlightCol);
    p.drawRectIn(rect, 1, MarginCol);
    // p.drawRectOut(rect, PieceMargins, Color::Black);
}
void PromotionScene::drawKeyboard(Paint& p, const Rect& rect)  {
    p.drawSprite(rect.p0(), sprites::Cursor, sprites::CursorPalette, 2);
}

void PromotionScene::onDraw(Paint& p) {
    ButtonSelectorScene::onDraw(p);
    std::clog << "PromotionScene.onDraw()\n";

    int i = 0;
    using namespace sprites;
    for (auto& sprite : {&Knight, &Bishop, &Rook, &Queen}) {
        auto pos = getButtonRect(i++).p0();
        p.drawSprite(pos, *sprite, chess::Piece::getPalette(side), 2);
    }
}

void PromotionScene::onButtonSelected(int index) {
    std::clog << "done: " << index << "\n";

    auto& b = MainScene::instance().getBoard();

    switch (index) {
    case 0: b.onGetPromotionResult(side, chess::PromotionResult::Knight); break;
    case 1: b.onGetPromotionResult(side, chess::PromotionResult::Bishop); break;
    case 2: b.onGetPromotionResult(side, chess::PromotionResult::Rook); break;
    case 3: b.onGetPromotionResult(side, chess::PromotionResult::Queen); break;
    default:
        std::cerr << "index = " << index;
        throw std::logic_error("invalid index");
    }
    SoundManager::playSelected();
    SceneManager::load<MainScene>();
}
void PromotionScene::onKeyDown(char k) {
    // switch (k) {
    // case VK_F1:
    //     warning("todo");
    //     break;
    // // case 'Q':
    // //     quit();
    // //     break;
    // }
    ButtonSelectorScene::onKeyDown(k);
}
